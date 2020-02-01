/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2017 - ROLI Ltd.

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 5 End-User License
   Agreement and JUCE 5 Privacy Policy (both updated and effective as of the
   27th April 2017).

   End User License Agreement: www.juce.com/juce-5-licence
   Privacy Policy: www.juce.com/juce-5-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "../UI/MainHostWindow.h"
#include "PluginGraph.h"
#include "InternalPlugins.h"
#include "../UI/GraphEditorPanel.h"
#include <windows.h>


//==============================================================================
PluginGraph::PluginGraph (AudioPluginFormatManager& fm, KnownPluginList& kpl)
    : FileBasedDocument (getFilenameSuffix(),
                         getFilenameWildcard(),
                         "Load a filter graph",
                         "Save a filter graph"),
      formatManager (fm), knownPluginList(kpl)
{
    newDocument();
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
        if (! graph.getNodes().contains (activePluginWindows.getUnchecked(i)->node))
            activePluginWindows.remove (i);
}



void PluginGraph::addPlugin (const PluginDescription& desc, Point<double> pos)
{
    formatManager.createPluginInstanceAsync (desc,
                                             graph.getSampleRate(),
                                             graph.getBlockSize(),
                                             [this, pos] (std::unique_ptr<AudioPluginInstance> instance, const String& error)
                                             {
                                                 addPluginCallback (std::move (instance), error, pos);
                                             });
}

void PluginGraph::addPluginCallback (std::unique_ptr<AudioPluginInstance> instance,
                                     const String& error, Point<double> pos)
{
    if (instance == nullptr)
    {
        AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                          TRANS("Couldn't create plugin"),
                                          error);
    }
    else
    {
        instance->enableAllBuses();

        if (auto node = graph.addNode (std::move (instance)))
        {
            node->properties.set ("x", pos.x);
            node->properties.set ("y", pos.y);
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

            if (description.pluginFormatName == "Internal")
            {
                getCommandManager().invokeDirectly (CommandIDs::showAudioSettings, false);
                return nullptr;
            }
        }

       #if JUCE_WINDOWS && JUCE_WIN_PER_MONITOR_DPI_AWARE
        if (! node->properties["DPIAware"]
            && ! node->getProcessor()->getName().contains ("Kontakt")) // Kontakt doesn't behave correctly in DPI unaware mode...
        {
            ScopedDPIAwarenessDisabler disableDPIAwareness;
            return activePluginWindows.add (new PluginWindow (node, type, activePluginWindows));
        }
       #endif

        return activePluginWindows.add (new PluginWindow (node, type, activePluginWindows));
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

    MessageManager::callAsync ([this] () {
        setChangedFlag (false);
        graph.addChangeListener (this);
    } );
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

    return Result::fail ("Not a valid performer file");
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

// shared code for load/import
void PluginGraph::setupPerformer()
{
    clear(); // because was in restoreFromXml

    InternalPluginFormat internalFormat;
    String errorMessage;
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
            auto name = knownPluginList.getType(j)->name;
            if (name.compareIgnoreCase(pd.name)==0 || name.compareIgnoreCase(pd.name.removeCharacters(" "))==0 || name.compareIgnoreCase(pd.name + " VSTi") == 0)
            {
                pd.fileOrIdentifier = knownPluginList.getType(j)->fileOrIdentifier;
                auto bankFile = File::getCurrentWorkingDirectory().getFullPathName() + "\\" + String(pd.name + "_Banks.txt");
                rack.m_usesBanks = File(bankFile).exists();
            }
        }

        auto processor = formatManager.createPluginInstance(pd, graph.getSampleRate(), graph.getBlockSize(), errorMessage);
        if (processor)
        {
            auto processorPtr = processor.get();

            if (rack.InitialState.size())
                SendChunkString(processorPtr, rack.InitialState);

            auto node = graph.addNode(std::unique_ptr<AudioProcessor>(std::move(processor)), (NodeID)rack.ID);
            auto midi = graph.addNode(formatManager.createPluginInstance(internalFormat.midiFilterDesc, graph.getSampleRate(), graph.getBlockSize(), errorMessage));
            auto gain = graph.addNode(formatManager.createPluginInstance(internalFormat.gainDesc, graph.getSampleRate(), graph.getBlockSize(), errorMessage));         

            rack.m_node = (void*)node.get();
            rack.m_gainNode = (void*)gain.get();
            rack.m_midiFilterNode = (void*)midi.get();

            graph.addConnection({ { m_midiControlNode->nodeID, 4096 },{ midi->nodeID, 4096 } });
            graph.addConnection({ { midi->nodeID, 4096 },{ node->nodeID, 4096 } });

            graph.addConnection({ { node->nodeID, 0 },{ gain->nodeID, 0 } });
            graph.addConnection({ { node->nodeID, 1 },{ gain->nodeID, 1 } });

            graph.addConnection({ { gain->nodeID, 0 },{ m_masterGainNode->nodeID, 0 } });
            graph.addConnection({ { gain->nodeID, 1 },{ m_masterGainNode->nodeID, 1 } });
        }
    }
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
    clear();
    m_performer.Import(filename);
    m_performer.ResolveIDs();
    graph.removeChangeListener (this);
    setupPerformer();
    MessageManager::callAsync ([this]
        {
            setChangedFlag (false);
            graph.addChangeListener (this);
        });
}


int m_pendingPerformanceIndex = 0;
int m_pendingSet = 0;

void PluginGraph::SetupKeylab(MidiBuffer &output, int sample_number)
{
    for (int m = 0; m<60; ++m)
    {
        unsigned char mes[10];
        int meslen = 0;
        memcpy(mes, "\x00\x20\x6B\x7F\x42\x02\x00", 7); meslen += 7;
        unsigned char parameter = 3;
        unsigned char control = 0;
        unsigned char value = 0;
        // order volume / knob 1 #3 / 9 disable knobs / Slider 1 #9 / 8 disable sliders / 16 pads (midi and note) / rewind (mode and cc) / forward (mode and cc) / stop (mode and cc)
        if (m == 0)
        {
            control = 0x30;
            value = 9;
        }
        else if (m == 1)
        {
            control = 1;
            value = 3;
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
            control = 0x59; // stop
            value = 8;
        }
        else if (m == 59)
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

void PluginGraph::UpdateLCDScreen(MidiBuffer &output, int sample_number)
{
    string songName1, songName2;


    PerformanceType* performance = NULL;
    Song *song = NULL;

    m_performer.GetPerformanceByIndex(performance, song);

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
    m_onProgramChange();
}

void PluginGraph::LoadSet(int setIndex)
{
    setIndex;
}

bool m_keylabNeedsSettingup = true;

void PluginGraph::Filter(int samples, int sampleRate, MidiBuffer &midiBuffer)
{
    if (m_keylabNeedsSettingup)
    {
        m_keylabNeedsSettingup = false;
        SetupKeylab(midiBuffer, 0);
        PrintLCDScreen(midiBuffer, 0, "Engine Loaded", "Select setlist");
    }
    samples;
    sampleRate;
    if (!midiBuffer.isEmpty())
    {
        MidiMessage midi_message;
        MidiBuffer output;
        int sample_number;

        MidiBuffer::Iterator midi_buffer_iter(midiBuffer);
        while (midi_buffer_iter.getNextEvent(midi_message, sample_number))
        {
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
                        UpdateLCDScreen(output, sample_number);
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
                InternalPluginFormat::SetGain(m_masterGainNode, ((float)midi_message.getControllerValue()) / 127.f, false);
            }
            else if (midi_message.isProgramChange())
            {
                MessageManagerLock ms;

                m_performer.m_currentPerformanceIndex = midi_message.getProgramChangeNumber();
                m_pendingPerformanceIndex = m_performer.m_currentPerformanceIndex;

                UpdateCurrentRouting();
            }
            else if (midi_message.isControllerOfType(111)) // backward
            {
                if (midi_message.getControllerValue() > 0)
                {
                    m_performer.m_currentPerformanceIndex--;
                    m_pendingPerformanceIndex = m_performer.m_currentPerformanceIndex;
                    UpdateCurrentRouting();
                }
                else
                    UpdateLCDScreen(output, sample_number);

            }
            else if (midi_message.isControllerOfType(116)) // forward
            {
                if (midi_message.getControllerValue() > 0)
                {
                    m_performer.m_currentPerformanceIndex++;
                    m_pendingPerformanceIndex = m_performer.m_currentPerformanceIndex;
                    UpdateCurrentRouting();
                }
                else
                    UpdateLCDScreen(output, sample_number);

            }
            else if (midi_message.isControllerOfType(115))
            {
                if (midi_message.getControllerValue() > 0)
                {
                    m_performer.m_currentPerformanceIndex = m_pendingPerformanceIndex;
                    UpdateCurrentRouting();
                }
                else
                    UpdateLCDScreen(output, sample_number); // just redraw
            }
            else if (midi_message.isControllerOfType(114) && midi_message.getControllerValue() == 0x3f) // anticlockwise
            {
                m_pendingPerformanceIndex--;
                UpdateLCDScreen(output, sample_number);
            }
            else if (midi_message.isControllerOfType(114) && midi_message.getControllerValue() == 0x41) // clockwise
            {
                m_pendingPerformanceIndex++;
                UpdateLCDScreen(output, sample_number);
            }
            else if (midi_message.isControllerOfType(113))
            {
                if (midi_message.getControllerValue() > 0)
                {
                    LoadSet(m_pendingSet);
                    UpdateLCDScreen(output, sample_number);
                }
                else
                    UpdateLCDScreen(output, sample_number); // just redraw
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
            else
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
        MidiMessage midi_message;
        MidiBuffer output;
        int sample_number;

        MidiBuffer::Iterator midi_buffer_iter(midiBuffer);
        while (midi_buffer_iter.getNextEvent(midi_message, sample_number))
        {
            if (midi_message.isSysEx())
                output.addEvent(midi_message, sample_number);
        }
        midiBuffer = output;
    }
}

void PluginGraph::CreateDefaultNodes()
{
    InternalPluginFormat internalFormat;
    String errorMessage;
    m_midiInNode = graph.addNode(formatManager.createPluginInstance(internalFormat.midiInDesc, graph.getSampleRate(), graph.getBlockSize(), errorMessage));
    m_midiOutNode = graph.addNode(formatManager.createPluginInstance(internalFormat.midiOutDesc, graph.getSampleRate(), graph.getBlockSize(), errorMessage));
    m_audioOutNode = graph.addNode(formatManager.createPluginInstance(internalFormat.audioOutDesc, graph.getSampleRate(), graph.getBlockSize(), errorMessage));
    m_midiControlNode = graph.addNode(formatManager.createPluginInstance(internalFormat.midiFilterDesc, graph.getSampleRate(), graph.getBlockSize(), errorMessage));
    m_midiSysexNode = graph.addNode(formatManager.createPluginInstance(internalFormat.midiFilterDesc, graph.getSampleRate(), graph.getBlockSize(), errorMessage));
    m_masterGainNode = graph.addNode(formatManager.createPluginInstance(internalFormat.gainDesc, graph.getSampleRate(), graph.getBlockSize(), errorMessage));

    InternalPluginFormat::SetFilterCallback(m_midiSysexNode, &m_nonSysexFilter);
    InternalPluginFormat::SetFilterCallback(m_midiControlNode, this);

    graph.addConnection({ { m_midiInNode->nodeID, 4096 },{ m_midiControlNode->nodeID, 4096 } });
    graph.addConnection({ { m_midiControlNode->nodeID, 4096 },{ m_midiSysexNode->nodeID, 4096 } });
    graph.addConnection({ { m_midiSysexNode->nodeID, 4096 },{ m_midiOutNode->nodeID, 4096 } });
    graph.addConnection({ { m_masterGainNode->nodeID, 0 },{ m_audioOutNode->nodeID, 0 } });
    graph.addConnection({ { m_masterGainNode->nodeID, 1 },{ m_audioOutNode->nodeID, 1 } });
}

