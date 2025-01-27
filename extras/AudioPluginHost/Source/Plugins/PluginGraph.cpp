/*
  ==============================================================================

   This file is part of the JUCE framework.
   Copyright (c) Raw Material Software Limited

   JUCE is an open source framework subject to commercial or open source
   licensing.

   By downloading, installing, or using the JUCE framework, or combining the
   JUCE framework with any other source code, object code, content or any other
   copyrightable work, you agree to the terms of the JUCE End User Licence
   Agreement, and all incorporated terms including the JUCE Privacy Policy and
   the JUCE Website Terms of Service, as applicable, which will bind you. If you
   do not agree to the terms of these agreements, we will not license the JUCE
   framework to you, and you must discontinue the installation or download
   process and cease use of the JUCE framework.

   JUCE End User Licence Agreement: https://juce.com/legal/juce-8-licence/
   JUCE Privacy Policy: https://juce.com/juce-privacy-policy
   JUCE Website Terms of Service: https://juce.com/juce-website-terms-of-service/

   Or:

   You may also use this code under the terms of the AGPLv3:
   https://www.gnu.org/licenses/agpl-3.0.en.html

   THE JUCE FRAMEWORK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL
   WARRANTIES, WHETHER EXPRESSED OR IMPLIED, INCLUDING WARRANTY OF
   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

#include <JuceHeader.h>
#include "../UI/MainHostWindow.h"
#include "PluginGraph.h"
#include "InternalPlugins.h"
#include <windows.h>
#include "../examples/Plugins/GainPluginDemo.h"

static std::unique_ptr<ScopedDPIAwarenessDisabler> makeDPIAwarenessDisablerForPlugin (const PluginDescription& desc)
{
    return shouldAutoScalePlugin (desc) ? std::make_unique<ScopedDPIAwarenessDisabler>()
                                        : nullptr;
}
class MyAudioPlayHead : public AudioPlayHead
{
public:
	virtual bool getCurrentPosition(CurrentPositionInfo& result)
	{
		result.bpm = m_bpm;
		return true;
	}

	double m_bpm = 120;
};

void PluginGraph::SetTempo(double tempo)
{
	((MyAudioPlayHead*)graph.getPlayHead())->m_bpm = tempo;
}

void PluginGraph::SetMono(bool mono)
{
	m_mono = mono;

	((AudioProcessorGraph::Node *)(m_masterGainNode))->getProcessor()->getParameters()[1]->setValue(mono ? 0.5f : 0.0f); // master mix

	for (auto i = 0U; i < m_performer.Root.Racks.Rack.size(); ++i)
	{
		auto &rack = m_performer.Root.Racks.Rack[i];
		if (rack.m_stereoToMonoWillPhase)
			((AudioProcessorGraph::Node *)(rack.m_gainNode))->getProcessor()->getParameters()[1]->setValue(mono ? 1.f : 0.f); // copy left to right or just leave
	}
}



//==============================================================================
PluginGraph::PluginGraph (AudioPluginFormatManager& fm, KnownPluginList& kpl)
    : FileBasedDocument (getFilenameSuffix(),
                         getFilenameWildcard(),
                         "Load a graph",
                         "Save a graph"),
      formatManager (fm),
      knownPlugins (kpl)
{
    newDocument();
	graph.setPlayHead(new MyAudioPlayHead());
    graph.addListener (this);
    m_shutdownPressCount = 0;
}

PluginGraph::~PluginGraph()
{
    graph.removeListener (this);
    graph.removeChangeListener (this);
    graph.clear();
}

PluginGraph::NodeID PluginGraph::getNextUID() noexcept
{
    return PluginGraph::NodeID (++(lastUID.uid));
}

//==============================================================================
void PluginGraph::changeListenerCallback (ChangeBroadcaster*)
{
    changed();

    for (int i = activePluginWindows.size(); --i >= 0;)
        if (! graph.getNodes().contains (activePluginWindows.getUnchecked (i)->node))
            activePluginWindows.remove (i);
}



void PluginGraph::addPlugin (const PluginDescriptionAndPreference& desc, Point<double> pos)
{
	String errorMessage;
	auto processor = formatManager.createPluginInstance(desc, graph.getSampleRate(), graph.getBlockSize(), errorMessage);

	Device newRack;
	newRack.ID = (int)Uuid().hash();
	newRack.Name = desc.name.getCharPointer();
	newRack.PluginName = desc.name.getCharPointer();

	if (processor.get())
		AddRack(processor, newRack);

	m_performer.Root.Racks.Rack.push_back(newRack);
	m_performer.ResolveIDs(); // Probably does more than required but doesnt matter

}

void PluginGraph::addPluginCallback (std::unique_ptr<AudioPluginInstance> instance,
                                     const String& error,
                                     Point<double> pos,
                                     PluginDescriptionAndPreference::UseARA useARA)
{
    if (instance == nullptr)
    {
        auto options = MessageBoxOptions::makeOptionsOk (MessageBoxIconType::WarningIcon,
                                                         TRANS ("Couldn't create plugin"),
                                                         error);
        messageBox = AlertWindow::showScopedAsync (options, nullptr);
    }
    else
    {
       #if JUCE_PLUGINHOST_ARA && (JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX)
        if (useARA == PluginDescriptionAndPreference::UseARA::yes
            && instance->getPluginDescription().hasARAExtension)
        {
            instance = std::make_unique<ARAPluginInstanceWrapper> (std::move (instance));
        }
       #endif

        instance->enableAllBuses();

        if (auto node = graph.addNode (std::move (instance)))
        {
            node->properties.set ("x", pos.x);
            node->properties.set ("y", pos.y);
            node->properties.set ("useARA", useARA == PluginDescriptionAndPreference::UseARA::yes);
            changed();
        }
    }
}


//==============================================================================
void PluginGraph::clear()
{
    closeAnyOpenPluginWindows();
    graph.clear();
    changed();
}

PluginWindow* PluginGraph::getOrCreateWindowFor (AudioProcessorGraph::Node* node, PluginWindow::Type type)
{
    jassert (node != nullptr);

   #if JUCE_IOS || JUCE_ANDROID
    closeAnyOpenPluginWindows();
   #else
    for (auto* w : activePluginWindows)
        if (w->node.get() == node && w->type == type)
            return w;
   #endif

    if (auto* processor = node->getProcessor())
    {
        if (auto* plugin = dynamic_cast<AudioPluginInstance*> (processor))
        {
            auto description = plugin->getPluginDescription();

            if (! plugin->hasEditor() && description.pluginFormatName == "Internal")
            {
                getCommandManager().invokeDirectly (CommandIDs::showAudioSettings, false);
                return nullptr;
            }

            auto localDpiDisabler = makeDPIAwarenessDisablerForPlugin (description);
            return activePluginWindows.add (new PluginWindow (node, type, activePluginWindows));
        }
    }

    return nullptr;
}

bool PluginGraph::closeAnyOpenPluginWindows()
{
    bool wasEmpty = activePluginWindows.isEmpty();
    activePluginWindows.clear();
    return ! wasEmpty;
}

//==============================================================================
String PluginGraph::getDocumentTitle()
{
    if (! getFile().exists())
        return "Unnamed";

    return getFile().getFileNameWithoutExtension();
}

void PluginGraph::newDocument()
{
    clear();
    setFile ({});

    graph.removeChangeListener (this);

    CreateDefaultNodes();

   
/*
    jassert (internalFormat.getAllTypes().size() > 3);

    addPlugin (PluginDescriptionAndPreference { internalFormat.getAllTypes()[0] }, { 0.5,  0.1 });
    addPlugin (PluginDescriptionAndPreference { internalFormat.getAllTypes()[1] }, { 0.25, 0.1 });
    addPlugin (PluginDescriptionAndPreference { internalFormat.getAllTypes()[2] }, { 0.5,  0.9 });
    addPlugin (PluginDescriptionAndPreference { internalFormat.getAllTypes()[3] }, { 0.25, 0.9 });
*/
    MessageManager::callAsync ([this]
    {
        setChangedFlag (false);
        graph.addChangeListener (this);
    });
}

Result PluginGraph::loadDocument (const File& file)
{
    m_performerFilename = file.getFileNameWithoutExtension().toStdString();
    XmlArchive::Load(file.getFullPathName().getCharPointer(), m_performer);
    {
        m_performer.ResolveIDs();
        graph.removeChangeListener (this);
        setupPerformer();
        MessageManager::callAsync ([this]
        {
            setChangedFlag (false);
            graph.addChangeListener (this);
        });

        return Result::ok();
    }

    //return Result::fail ("Not a valid performer file");
}

Result PluginGraph::saveDocument (const File& file)
{
    XmlArchive::Save(file.getFullPathName().getCharPointer(), m_performer);
    return Result::ok();
}

static bool compareMagic(int32 magic, const char* name) noexcept
{
    return magic == (int32)ByteOrder::littleEndianInt(name)
        || magic == (int32)ByteOrder::bigEndianInt(name);
}


void PluginGraph::SendChunkString(AudioPluginInstance *processorPtr, StringRef str)
{
    MemoryOutputStream output;
    Base64::convertFromBase64(output, str);

    MemoryInputStream compressedInput(output.getData(), output.getDataSize(), false);
    GZIPDecompressorInputStream unzipper(compressedInput);
    MemoryOutputStream output2;
    output2 << unzipper;

    auto progAtBeginning = (memcmp(output2.getData(), "Prog", 4) == 0);

    struct fxSet
    {
        int32 chunkMagic;    // 'CcnK'
        int32 byteSize;      // of this chunk, excl. magic + byteSize
        int32 fxMagic;       // 'FxBk'
    };

    struct fxChunkSet
    {
        int32 chunkMagic;    // 'CcnK'
        int32 byteSize;      // of this chunk, excl. magic + byteSize
        int32 fxMagic;       // 'FxCh', 'FPCh', or 'FBCh'
        int32 version;
        int32 fxID;          // fx unique id
        int32 fxVersion;
        int32 numPrograms;
        char future[128];
        int32 chunkSize;
        char chunk[8];          // variable
    };

    struct fxProgramSet
    {
        int32 chunkMagic;    // 'CcnK'
        int32 byteSize;      // of this chunk, excl. magic + byteSize
        int32 fxMagic;       // 'FxCh', 'FPCh', or 'FBCh'
        int32 version;
        int32 fxID;          // fx unique id
        int32 fxVersion;
        int32 numPrograms;
        char name[28];
        int32 chunkSize;
        char chunk[8];          // variable
    };

    auto set = (const fxSet*)((uint64_t)output2.getData() + (progAtBeginning ? 16 : 0));

    // setStateInformation sets program name so don't want that
    if (compareMagic(set->fxMagic, "FBCh"))
    {
        // non-preset chunk
        auto cset = (const fxChunkSet*)set;
        VSTPluginFormat::setChunkData(processorPtr, cset->chunk, (int32)ByteOrder::swapIfLittleEndian((uint32)cset->chunkSize), false);
    }
    else if (compareMagic(set->fxMagic, "FPCh"))
    {
        auto cset = (const fxProgramSet*)set;
        VSTPluginFormat::setChunkData(processorPtr, cset->chunk, (int32)ByteOrder::swapIfLittleEndian((uint32)cset->chunkSize), true);
    }
}

PluginDescription FindInternalPlugin(std::vector<PluginDescription>& types, const char* name)
{
	for (int i = 0; i < types.size(); ++i)
		if (types[i].name == name)
			return types[i];
	return PluginDescription();
}

void PluginGraph::AddRack(std::unique_ptr<AudioPluginInstance> &processor, Device &rack)
{
	auto bankFile = File::getCurrentWorkingDirectory().getFullPathName() + "\\" + String(rack.PluginName + "_Banks.txt");
	rack.m_usesBanks = File(bankFile).exists();
	rack.m_stereoToMonoWillPhase = rack.PluginName =="TruePianos x64" || rack.PluginName == "P8";

	String errorMessage;

	InternalPluginFormat internalFormat;
	auto types = internalFormat.getAllTypes();

	processor->setPlayHead(graph.getPlayHead());

    auto audioInputs = processor->getTotalNumInputChannels();
    if (String(rack.PluginName).contains("DirectWave"))
        audioInputs = 0;

    auto node = graph.addNode(std::unique_ptr<AudioProcessor>(std::move(processor)), (NodeID)rack.ID);
	auto midi = graph.addNode(formatManager.createPluginInstance(FindInternalPlugin(types, "Midi Filter"), graph.getSampleRate(), graph.getBlockSize(), errorMessage));
	auto gain = graph.addNode(formatManager.createPluginInstance(FindInternalPlugin(types, "Gain PlugIn"), graph.getSampleRate(), graph.getBlockSize(), errorMessage));

    rack.m_node = (void*)node.get();
	rack.m_gainNode = (void*)gain.get();
	rack.m_midiFilterNode = (void*)midi.get();

	graph.addConnection({ { m_midiControlNode->nodeID, 4096 },{ midi->nodeID, 4096 } });
	graph.addConnection({ { midi->nodeID, 4096 },{ node->nodeID, 4096 } });

	graph.addConnection({ { node->nodeID, 0 },{ gain->nodeID, 0 } });
	graph.addConnection({ { node->nodeID, 1 },{ gain->nodeID, 1 } });

	graph.addConnection({ { gain->nodeID, 0 },{ m_masterGainNode->nodeID, 0 } });
	graph.addConnection({ { gain->nodeID, 1 },{ m_masterGainNode->nodeID, 1 } });

    if (audioInputs > 0)
    {
        graph.addConnection({ { m_audioInNode->nodeID, 0 }, { node->nodeID, 0 }});
        graph.addConnection({ { m_audioInNode->nodeID, 1 }, { node->nodeID, 1 }});
    }
}

// shared code for load/import
void PluginGraph::setupPerformer()
{
    clear(); // because was in restoreFromXml

    CreateDefaultNodes();

    for (auto i = 0U; i < m_performer.Root.Racks.Rack.size(); ++i)
    {
        auto &rack = m_performer.Root.Racks.Rack[i];

        PluginDescription pd;
        pd.name = rack.PluginName;
        pd.pluginFormatName = "VST";
        pd.isInstrument = true;
        for (auto j = 0U; j < (unsigned)knownPluginList.getNumTypes(); ++j)
        {
            auto name = knownPluginList.getTypes()[j].name;
            if (name.compareIgnoreCase(pd.name)==0 || name.compareIgnoreCase(pd.name.removeCharacters(" "))==0 || name.compareIgnoreCase(pd.name + " VSTi") == 0)
                pd.fileOrIdentifier = knownPluginList.getTypes()[j].fileOrIdentifier;
        }

		String errorMessage;
        auto processor = formatManager.createPluginInstance(pd, graph.getSampleRate(), graph.getBlockSize(), errorMessage);

        int headerSize = 0;
        String dir;
        vector<string> banks;
        int patches = 128;
        if (rack.PluginName == "JV-1080(VST2 64bit)")
        {
            headerSize = 25;
            dir = "JV-1080";
            banks.push_back("PR-A");
            banks.push_back("PR-B");
            banks.push_back("PR-C");
            banks.push_back("PR-D");
            banks.push_back("PR-E");
        }
        if (rack.PluginName == "JUPITER-8(VST2 64bit)")
        {
            headerSize = 22;
            dir = "JUPITER-8";
            banks.push_back("1 Preset");
            banks.push_back("Jupiter-8Techno");
            patches = 64;
        }

        if (headerSize)
        {
            rack.m_overridePatches.resize(banks.size());
            for (int b = 0; b < rack.m_overridePatches.size(); ++b)
            {
                auto input = File(String("C:\\ProgramData\\Roland Cloud\\")+ dir +"\\" + banks [b]+".bin").createInputStream();
                MemoryBlock memblock;
                input->readIntoMemoryBlock(memblock);

                char* ptr = (char*)memblock.getData() + headerSize;

                for (int p = 0; p < patches; ++p)
                {
                    String name = ptr;

                    name = name.upToFirstOccurrenceOf("\x1", false, false);
                        
                    rack.m_overridePatches[b].push_back(name.trimEnd().toStdString());
                    ptr += (memblock.getSize() - headerSize) / patches;
                }
            }
        }

        if (auto processorPtr = processor.get())
        {

            if (rack.PluginName == "TRITON")
            {
                auto input = File(pd.fileOrIdentifier.getCharPointer()).createInputStream();
                MemoryBlock memblock;
                input->readIntoMemoryBlock(memblock);

                char* ptr = (char*)memblock.getData();
                const char* firstPatch = "Noisy Stabber";
                int firstPatchLen = (int)strlen(firstPatch);
                char* endPtr = (char*)memblock.getData() + memblock.getSize() - strlen(firstPatch) - 1;
                int bankRemap[] = { 0,1,2,3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,8,9,10,11,12,13,14,15,16,-1,-1,-1,-1,-1,-1,-1,32,33,34,35 };
                int numBanks = sizeof(bankRemap) / sizeof(bankRemap[0]);
                while (ptr < endPtr)
                {
                    if (memcmp(ptr, firstPatch, firstPatchLen) == 0)
                    {
                        rack.m_overridePatches.resize(numBanks);
                        for (int b = 0; b < numBanks; ++b)
                        {
                            if (bankRemap[b] == -1)
                            {
                                ptr += 22 * 128;
                                continue;

                            }
                            for (int p = 0; p < 128; ++p)
                            {
                                String name = ptr;
                                rack.m_overridePatches[bankRemap[b]].push_back(name.trimEnd().toStdString());
                                ptr += 22;
                            }
                        }
                        break;
                    }
                    ptr++;
                }
            }


			if (rack.InitialState.size())
				SendChunkString(processorPtr, rack.InitialState);
			
			AddRack(processor, rack);
        }
    }
    m_keylabReady = true;
}

File PluginGraph::getLastDocumentOpened()
{
    RecentlyOpenedFilesList recentFiles;
    recentFiles.restoreFromString (getAppProperties().getUserSettings()
                                        ->getValue ("recentFilterGraphFiles"));

    return recentFiles.getFile (0);
}

void PluginGraph::setLastDocumentOpened (const File& file)
{
    RecentlyOpenedFilesList recentFiles;
    recentFiles.restoreFromString (getAppProperties().getUserSettings()
                                        ->getValue ("recentFilterGraphFiles"));

    recentFiles.addFile (file);

    getAppProperties().getUserSettings()
        ->setValue ("recentFilterGraphFiles", recentFiles.toString());
}

void PluginGraph::Import(const char *filename)
{
	graph.removeChangeListener(this);
	
	m_performer = Performer();
    m_performer.Import(filename);
    m_performer.ResolveIDs();

	clear(); // Do this after import
	
	setupPerformer();
    MessageManager::callAsync ([this]
        {
            setChangedFlag (false);
            graph.addChangeListener (this);
        });
}


int m_pendingPerformanceIndex = 0;
int m_pendingSet = 0;

// https://forum.arturia.com/index.php?topic=87454.msg153922#msg153922
void PluginGraph::SetupKeylab(MidiBuffer &output, int sample_number)
{
    for (int m = 0; m<64; ++m)
    {
        unsigned char mes[10];
        int meslen = 0;
        memcpy(mes, "\x00\x20\x6B\x7F\x42\x02\x00", 7); meslen += 7;
        unsigned char parameter = 3;
        unsigned char control = 0;
        unsigned char value = 0;
        // order volume / knob 1 #16 / 9 disable knobs / Slider 1 #9 / 8 disable sliders / 16 pads (midi and note) / rewind (mode and cc) / forward (mode and cc) / stop (mode and cc)
        if (m == 0)
        {
            control = 0x30;
            value = 9;
        }
        else if (m == 1)
        {
            control = 1;
            value = 16;
        }
        else if (m<2 + 9)
        {
            parameter = 1; // mode
            control = (unsigned char)m; // knob 2 to knob 10
            value = 0; // disable
        }
        else if (m == 2 + 9)
        {
            control = 0x0b; // slider 1
            value = 9; // also setup slider 1 like volume
        }
        else if (m<2 + 9 + 1 + 8)
        {
            parameter = 5; // max val
            control = (unsigned char)((m<2 + 1 + 8 + 4) ? m : (m + 0x3c));
            value = 0; // another way of disable (didn't work other way)
        }
        else if (m<2 + 9 + 1 + 8 + 16)
        {
            parameter = 2; // midi channel (defaults to 10 on pads)
            control = (unsigned char)(0x70 + (m - 2 - 9 - 1 - 8));
            value = 0; // midi channel 1
        }
        else if (m<2 + 9 + 1 + 8 + 16 * 2)
        {
            control = (unsigned char)(0x70 + (m - 2 - 9 - 1 - 8 - 16));
            value = (unsigned char)(0x15 + (m - 2 - 9 - 1 - 8 - 16)); // Low A up
        }
        else if (m == 54)
        {
            parameter = 1;
            control = 0x5b; // rewind
            value = 8; // for some reason this mode value instead of 3
        }
        else if (m == 55)
        {
            parameter = 3;
            control = 0x5b;
            value = 111;
        }
        else if (m == 56)
        {
            parameter = 1;
            control = 0x5c; // forward
            value = 8;
        }
        else if (m == 57)
        {
            parameter = 3;
            control = 0x5c;
            value = 116;
        }
        else if (m == 58)
        {
            parameter = 1;
            control = 0x58; // rewind backup
            value = 8; // for some reason this mode value instead of 3
        }
        else if (m == 59)
        {
            parameter = 3;
            control = 0x58;
            value = 111;
        }
        else if (m == 60)
        {
            parameter = 1;
            control = 0x5a; // forward backup (record)
            value = 8;
        }
        else if (m == 61)
        {
            parameter = 3;
            control = 0x5a;
            value = 116;
        }
        else if (m == 62)
        {
            parameter = 1;
            control = 0x59; // stop
            value = 8;
        }
        else if (m == 63)
        {
            parameter = 3;
            control = 0x59;
            value = 117;
        }


        mes[meslen++] = parameter;
        mes[meslen++] = control;
        mes[meslen++] = value;

        // send to lcd
        vector<unsigned char> message(meslen);
        memcpy(message.data(), mes, meslen);
        output.addEvent(MidiMessage::createSysExMessage(mes, meslen), sample_number);
    }
}


void PluginGraph::PrintLCDScreen(MidiBuffer &output, int sample_number, const char *text1, const char *text2)
{
    std::string ip;

    if (strcmp(text1, " ") == 0 && strcmp(text2, " ") == 0)
    {
#ifdef WIN32
        WSADATA wsa;
        WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
        char szHostName[255];
        gethostname(szHostName, 255);
        struct hostent *host_entry;
        host_entry = gethostbyname(szHostName);
        text1 = szHostName;
        ip = inet_ntoa(*(struct in_addr *)*host_entry->h_addr_list);
        if (ip.length() <= 13)
            ip = "IP:" + ip;
        text2 = ip.c_str();
#ifdef WIN32
        WSACleanup();
#endif
    }

    unsigned char mes[12 + 16 * 2];
    int meslen = 0;
    memcpy(mes, "\x00\x20\x6B\x7F\x42\x04\x00\x60\x01", 9); meslen += 9;
    memcpy(mes + meslen, text1, strlen(text1) + 1); meslen += (int)strlen(text1) + 1;
    mes[meslen++] = 0x02;
    memcpy(mes + meslen, text2, strlen(text2) + 1); meslen += (int)strlen(text2) + 1;

    // send to lcd
    output.addEvent(MidiMessage::createSysExMessage(mes, meslen), sample_number);
}




void OptimizeLines(string &songName1, string &songName2)
{
    bool line2free = (songName2 == "");
    while (songName1.size()>16 && line2free) // nothing on line 2 and line 1 is too long, try putting it to next line
    {
        if (char* ptr = (char*)strrchr(songName1.c_str(), ' '))
        {
            songName2 = std::string(ptr + 1) + " " + songName2;
            songName1.resize(ptr - songName1.c_str());
        }
    }

    TrimRight(songName2); // procedure above may have added space

                          // if still not fit, remove spaces
    if (songName1.size()>16 || songName2.size()>16)
    {
        Replace(songName1, " ", "");
        if (line2free)
            Replace(songName2, " ", "");
    }

    // oh well trunc
    if (songName1.size()>16)
        songName1.resize(16);
    if (songName2.size()>16)
        songName2.resize(16);
    if (songName1 == "")
        songName1 = " ";
    if (songName2 == "")
        songName2 = " ";
}

void PluginGraph::UpdateLCDScreen(MidiBuffer &output, int sample_number, int index)
{
    string songName1, songName2;


    PerformanceType* performance = NULL;
    Song *song = NULL;

    m_performer.GetPerformanceByIndex(performance, song, index);

    songName1 = song->Name;
    songName2 = performance->Name;

    size_t found = songName2.find("|");
    if (found != -1)
        songName2 = string(songName2.c_str() + found + 1);
    TrimRight(songName2);

    if (songName1 == songName2)
        songName2 = "";


    OptimizeLines(songName1, songName2);



    if (m_performer.Root.SetLists.SetList.size()>0)
        PrintLCDScreen(output, sample_number, songName1.c_str(), songName2.c_str());
    else
        PrintLCDScreen(output, sample_number, "No set loaded", " ");
}

void PluginGraph::UpdateCurrentRouting()
{
    MessageManagerLock ms;
    m_onProgramChange();
}

void PluginGraph::LoadSet(int setIndex)
{
    // TODO
    setIndex; 
    m_performer.m_currentPerformanceIndex = 0;
}
    }

void PluginGraph::Filter(int samples, int sampleRate, MidiBuffer &midiBuffer)
{
    if (m_keylabNeedsSettingup)
    {
        m_keylabNeedsSettingup = false;
        if (!m_isKeylab88MkII) // We'll use "Analog Lab" mode so no controller assignment, remap at runtime
            SetupKeylab(midiBuffer, 0);
        PrintLCDScreen(midiBuffer, 0, "Performer", "Loading...");
    }

    if (m_keylabReady)
    {
        MidiBuffer output;
        m_keylabReady = false;
        UpdateLCDScreen(output, 0, m_performer.m_currentPerformanceIndex);
        midiBuffer = output;
    }

    samples;
    sampleRate;

    if (m_manualMidi > 0)
    {
        midiBuffer.addEvent(MidiMessage::controllerEvent(1, m_manualMidi, 127),0);
        m_manualMidi = 0;
    }

    if (!midiBuffer.isEmpty())
    {
        MidiBuffer output;

		for (const auto meta : midiBuffer)
		{
			auto midi_message = meta.getMessage();
			int sample_number = meta.samplePosition;

            if (m_isKeylab88MkII && midi_message.isController())
            {
                // Remap for Keylab MKII because want it in "Analog Lab" mode to change patches and can't override controllers
                if (midi_message.getControllerNumber() == 0x4A)
                    midi_message = MidiMessage::controllerEvent(midi_message.getChannel(), 16, midi_message.getControllerValue());
                else if (midi_message.getControllerNumber() == 0x55)
                    midi_message = MidiMessage::controllerEvent(midi_message.getChannel(), 9, midi_message.getControllerValue());
                else if (midi_message.getControllerNumber() == 0x1C)
                    midi_message = MidiMessage::controllerEvent(midi_message.getChannel(), 111, midi_message.getControllerValue()); // back
                else if (midi_message.getControllerNumber() == 0x1D)
                    midi_message = MidiMessage::controllerEvent(midi_message.getChannel(), 116, midi_message.getControllerValue()); // forward
                else if (midi_message.getControllerNumber() == 0x1E && (midi_message.getControllerValue() == 0x10 || midi_message.getControllerValue() == 0x11))
                    midi_message = MidiMessage::controllerEvent(midi_message.getChannel(), 117, (midi_message.getControllerValue() == 0x11) ? 127 : 0); // power off
            }

            if (midi_message.isControllerOfType(117)) // power off
            {
                if (midi_message.getControllerValue() == 127)
                    m_shutdownPressCount++;
                if (m_shutdownPressCount == 1)
                    PrintLCDScreen(output, sample_number, "Are you sure?", " ");
                if (m_shutdownPressCount > 1)
                {
                    PrintLCDScreen(output, sample_number, "Shutting Down", " ");
                    system("shutdown /t 0 /s");
                }
            }
            else
            {
                if (m_shutdownPressCount) // to remove confirm text
                {
                    if (m_performer.Root.SetLists.SetList.size() > 0)
                        UpdateLCDScreen(output, sample_number, m_performer.m_currentPerformanceIndex);
                    else
                        PrintLCDScreen(output, sample_number, " ", " ");
                }
                m_shutdownPressCount = 0;

            }

            if (midi_message.isControllerOfType(0) || midi_message.isControllerOfType(0x20))
            {
                // ignore bank change 
            }
            else if (midi_message.isAftertouch() || midi_message.isChannelPressure())
            {
                //ignore aftertouch
            }
            //else if (midi_message.isPitchWheel())
            //{
            //  //ignore pitch bend
            //}
            else if (midi_message.isControllerOfType(0x07))
            {
                //ignore volume changes
            }
            //else if (midi_message.isControllerOfType(0x01))
            //{
            //  //ignore modulation
            //}
            else if (midi_message.isControllerOfType(0x09))
            {
				((AudioProcessorGraph::Node *)(m_masterGainNode))->getProcessor()->getParameters()[0]->setValue((((float)midi_message.getControllerValue()) / 127.f) * 0.25f);
            }
            else if (midi_message.isProgramChange())
            {
                m_performer.m_currentPerformanceIndex = midi_message.getProgramChangeNumber();
                m_pendingPerformanceIndex = m_performer.m_currentPerformanceIndex;

                UpdateCurrentRouting();
            }
            else if (midi_message.isControllerOfType(111)) // backward
            {
                if (midi_message.getControllerValue() > 0)
                {
                    m_performer.m_currentPerformanceIndex--;
					if (m_performer.m_currentPerformanceIndex < 0)
						m_performer.m_currentPerformanceIndex = 0;
                    m_pendingPerformanceIndex = m_performer.m_currentPerformanceIndex;
                    UpdateCurrentRouting();
                    UpdateLCDScreen(output, sample_number, m_performer.m_currentPerformanceIndex);
                }
                else
                    UpdateLCDScreen(output, sample_number, m_performer.m_currentPerformanceIndex); // just redraw

            }
            else if (midi_message.isControllerOfType(116)) // forward
            {
                if (midi_message.getControllerValue() > 0)
                {
                    m_performer.m_currentPerformanceIndex++;
                    m_pendingPerformanceIndex = m_performer.m_currentPerformanceIndex;
                    UpdateCurrentRouting();
                    UpdateLCDScreen(output, sample_number, m_performer.m_currentPerformanceIndex);
                }
                else
                    UpdateLCDScreen(output, sample_number, m_performer.m_currentPerformanceIndex); // just redraw

            }
            else if (midi_message.isControllerOfType(115))
            {
                if (midi_message.getControllerValue() > 0)
                {
                    m_performer.m_currentPerformanceIndex = m_pendingPerformanceIndex;
                    UpdateCurrentRouting();
                }
                else
                    UpdateLCDScreen(output, sample_number, m_performer.m_currentPerformanceIndex); // just redraw
            }
            else if (midi_message.isControllerOfType(114) && midi_message.getControllerValue() == 0x3f) // anticlockwise
            {
                m_pendingPerformanceIndex--;
                UpdateLCDScreen(output, sample_number, m_pendingPerformanceIndex);
            }
            else if (midi_message.isControllerOfType(114) && midi_message.getControllerValue() == 0x41) // clockwise
            {
                m_pendingPerformanceIndex++;
                UpdateLCDScreen(output, sample_number, m_pendingPerformanceIndex);
            }
            else if (midi_message.isControllerOfType(113))
            {
                if (midi_message.getControllerValue() > 0)
                {
                    LoadSet(m_pendingSet);
                    UpdateLCDScreen(output, sample_number, m_performer.m_currentPerformanceIndex);
                }
                else
                    UpdateLCDScreen(output, sample_number, m_performer.m_currentPerformanceIndex); // just redraw
            }
            else if (midi_message.isControllerOfType(112) && midi_message.getControllerValue() == 0x3f) // category anticlockwise
            {
                m_pendingSet--;
                if (m_pendingSet < 0)
                    m_pendingSet = (int)m_performer.Root.SetLists.SetList.size() - 1;
                if (m_pendingSet < 0)
                    m_pendingSet = 0;
                if (m_performer.Root.SetLists.SetList.size() > 0)
                    PrintLCDScreen(output, sample_number, m_performerFilename.c_str(), m_performer.Root.SetLists.SetList[m_pendingSet].Name.c_str());
            }
            else if (midi_message.isControllerOfType(112) && midi_message.getControllerValue() == 0x41) // category clockwise
            {
                m_pendingSet++;
                if (m_pendingSet >= (int)m_performer.Root.SetLists.SetList.size())
                    m_pendingSet = 0;
                if (m_performer.Root.SetLists.SetList.size() > 0)
                    PrintLCDScreen(output, sample_number, m_performerFilename.c_str(), m_performer.Root.SetLists.SetList[m_pendingSet].Name.c_str());
            }
            else if (midi_message.isNoteOn() && midi_message.getVelocity() == 0)
                output.addEvent(MidiMessage::noteOff(midi_message.getChannel(), midi_message.getNoteNumber()), sample_number);
            else if ((midi_message.isController() && (midi_message.getControllerNumber() == 1/*modulation*/ || midi_message.getControllerNumber() == 16 || midi_message.getControllerNumber() == 0x40/*sustain*/ || midi_message.isPitchWheel())) || !midi_message.isController())
                output.addEvent(midi_message, sample_number);
        }
        midiBuffer = output;
    }
}

void NonSysexFilter::Filter(int samples, int sampleRate, MidiBuffer &midiBuffer)
{
    samples;
    sampleRate;
    if (!midiBuffer.isEmpty())
    {
        MidiBuffer output;

		for (const auto meta : midiBuffer)
		{
			auto midi_message = meta.getMessage();
			int sample_number = meta.samplePosition;

            if (midi_message.isSysEx())
                output.addEvent(midi_message, sample_number);
        }
        midiBuffer = output;
    }
}

void PluginGraph::CreateDefaultNodes()
{
    InternalPluginFormat internalFormat;
	auto types = internalFormat.getAllTypes();

    String errorMessage;
    m_midiInNode = graph.addNode(formatManager.createPluginInstance(FindInternalPlugin(types,"MIDI Input"), graph.getSampleRate(), graph.getBlockSize(), errorMessage));
    m_midiOutNode = graph.addNode(formatManager.createPluginInstance(FindInternalPlugin(types, "MIDI Output"), graph.getSampleRate(), graph.getBlockSize(), errorMessage));
    m_audioInNode = graph.addNode(formatManager.createPluginInstance(FindInternalPlugin(types, "Audio Input"), graph.getSampleRate(), graph.getBlockSize(), errorMessage));
    m_audioOutNode = graph.addNode(formatManager.createPluginInstance(FindInternalPlugin(types, "Audio Output"), graph.getSampleRate(), graph.getBlockSize(), errorMessage));
    m_midiControlNode = graph.addNode(formatManager.createPluginInstance(FindInternalPlugin(types, "Midi Filter"), graph.getSampleRate(), graph.getBlockSize(), errorMessage));
    m_midiSysexNode = graph.addNode(formatManager.createPluginInstance(FindInternalPlugin(types, "Midi Filter"), graph.getSampleRate(), graph.getBlockSize(), errorMessage));
    m_masterGainNode = graph.addNode(formatManager.createPluginInstance(FindInternalPlugin(types, "Gain PlugIn"), graph.getSampleRate(), graph.getBlockSize(), errorMessage));

    InternalPluginFormat::SetFilterCallback(m_midiSysexNode, &m_nonSysexFilter);
    InternalPluginFormat::SetFilterCallback(m_midiControlNode, this);

    graph.addConnection({ { m_midiInNode->nodeID, 4096 },{ m_midiControlNode->nodeID, 4096 } });
    graph.addConnection({ { m_midiControlNode->nodeID, 4096 },{ m_midiSysexNode->nodeID, 4096 } });
    graph.addConnection({ { m_midiSysexNode->nodeID, 4096 },{ m_midiOutNode->nodeID, 4096 } });
    graph.addConnection({ { m_masterGainNode->nodeID, 0 },{ m_audioOutNode->nodeID, 0 } });
    graph.addConnection({ { m_masterGainNode->nodeID, 1 },{ m_audioOutNode->nodeID, 1 } });
}

