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
#ifdef JUCE_WINDOWS
#include <windows.h>
#else
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include "../examples/Plugins/GainPluginDemo.h"

static std::unique_ptr<ScopedDPIAwarenessDisabler> makeDPIAwarenessDisablerForPlugin (const PluginDescription& desc)
{
    return shouldAutoScalePlugin (desc) ? std::make_unique<ScopedDPIAwarenessDisabler>()
                                        : nullptr;
}

class MyAudioPlayHead : public AudioPlayHead
{
public:
   Optional<PositionInfo> getPosition() const override
	{
        PositionInfo result;
		result.setBpm(m_bpm);
        result.setTimeInSamples(0); // Setting this to 0 but should probably be a proper value
		return result;
	}

    static double m_bpm;
};

double MyAudioPlayHead::m_bpm = 120;

void PluginGraph::SetTempo(double tempo)
{
	 MyAudioPlayHead::m_bpm = tempo;
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

/*
AudioProcessorGraph::Node::Ptr PluginGraph::getNodeForName (const String& name) const
{
    for (auto* node : graph.getNodes())
        if (auto p = node->getProcessor())
            if (p->getName().equalsIgnoreCase (name))
                return node;

    return nullptr;
}
*/

void PluginGraph::addPlugin (const PluginDescriptionAndPreference& desc, Point<double> pos)
{
    std::shared_ptr<ScopedDPIAwarenessDisabler> dpiDisabler = makeDPIAwarenessDisablerForPlugin (desc.pluginDescription);


    /*
    formatManager.createPluginInstanceAsync (desc.pluginDescription,
                                             graph.getSampleRate(),
                                             graph.getBlockSize(),
                                             [this, pos, dpiDisabler, useARA = desc.useARA] (std::unique_ptr<AudioPluginInstance> instance, const String& error)
                                             {
                                                 addPluginCallback (std::move (instance), error, pos, useARA);
                                             });
    */
    pos; // use

    CreateDefaultNodes(); // Couldn't hear anything after adding a new plugin until this

    String errorMessage;
	auto processor = formatManager.createPluginInstance(desc.pluginDescription, graph.getSampleRate(), graph.getBlockSize(), errorMessage);

	Device newRack;
	newRack.ID = (int)Uuid().hash();
	newRack.PluginName = desc.pluginDescription.name.getCharPointer();
    newRack.m_order = m_performer.Root.Racks.Rack.size();

    String name = desc.pluginDescription.name.getCharPointer();
    name = name.replace(" x64", "");
    name = name.replace("(VST2 64bit)", "");
    if (name == "M1")
        name = "Korg M1";
    else if (name == "WAVESTATION")
        name = "Korg Wavestation";
    else if (name == "TRITON")
        name = "Korg Triton";
    else if (name == "JV-1080")
        name = "Roland JV-1080";
    else if (name == "JUPITER-8")
        name = "Roland Jupiter-8";
    newRack.Name = name.toStdString();

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

/*
void PluginGraph::setNodePosition (NodeID nodeID, Point<double> pos)
{
    if (auto* n = graph.getNodeForId (nodeID))
    {
        n->properties.set ("x", jlimit (0.0, 1.0, pos.x));
        n->properties.set ("y", jlimit (0.0, 1.0, pos.y));
    }
}

Point<double> PluginGraph::getNodePosition (NodeID nodeID) const
{
    if (auto* n = graph.getNodeForId (nodeID))
        return { static_cast<double> (n->properties ["x"]),
                 static_cast<double> (n->properties ["y"]) };

    return {};
}
*/

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
    InternalPluginFormat internalFormat;

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

    return Result::fail ("Not a valid performer file");
}

Result PluginGraph::saveDocument (const File& file)
{
    XmlArchive::Save(file.getFullPathName().getCharPointer(), m_performer);
    return Result::ok();
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

/*
//==============================================================================
static void readBusLayoutFromXml (AudioProcessor::BusesLayout& busesLayout, AudioProcessor& plugin,
                                  const XmlElement& xml, bool isInput)
{
    auto& targetBuses = (isInput ? busesLayout.inputBuses
                                 : busesLayout.outputBuses);
    int maxNumBuses = 0;

    if (auto* buses = xml.getChildByName (isInput ? "INPUTS" : "OUTPUTS"))
    {
        for (auto* e : buses->getChildWithTagNameIterator ("BUS"))
        {
            const int busIdx = e->getIntAttribute ("index");
            maxNumBuses = jmax (maxNumBuses, busIdx + 1);

            // the number of buses on busesLayout may not be in sync with the plugin after adding buses
            // because adding an input bus could also add an output bus
            for (int actualIdx = plugin.getBusCount (isInput) - 1; actualIdx < busIdx; ++actualIdx)
                if (! plugin.addBus (isInput))
                    return;

            for (int actualIdx = targetBuses.size() - 1; actualIdx < busIdx; ++actualIdx)
                targetBuses.add (plugin.getChannelLayoutOfBus (isInput, busIdx));

            auto layout = e->getStringAttribute ("layout");

            if (layout.isNotEmpty())
                targetBuses.getReference (busIdx) = AudioChannelSet::fromAbbreviatedString (layout);
        }
    }

    // if the plugin has more buses than specified in the xml, then try to remove them!
    while (maxNumBuses < targetBuses.size())
    {
        if (! plugin.removeBus (isInput))
            return;

        targetBuses.removeLast();
    }
}

//==============================================================================
static XmlElement* createBusLayoutXml (const AudioProcessor::BusesLayout& layout, const bool isInput)
{
    auto& buses = isInput ? layout.inputBuses
                          : layout.outputBuses;

    auto* xml = new XmlElement (isInput ? "INPUTS" : "OUTPUTS");

    for (int busIdx = 0; busIdx < buses.size(); ++busIdx)
    {
        auto& set = buses.getReference (busIdx);

        auto* bus = xml->createNewChildElement ("BUS");
        bus->setAttribute ("index", busIdx);
        bus->setAttribute ("layout", set.isDisabled() ? "disabled" : set.getSpeakerArrangementAsString());
    }

    return xml;
}

static XmlElement* createNodeXml (AudioProcessorGraph::Node* const node) noexcept
{
    if (auto* plugin = dynamic_cast<AudioPluginInstance*> (node->getProcessor()))
    {
        auto e = new XmlElement ("FILTER");

        e->setAttribute ("uid",      (int) node->nodeID.uid);
        e->setAttribute ("x",        node->properties ["x"].toString());
        e->setAttribute ("y",        node->properties ["y"].toString());
        e->setAttribute ("useARA",   node->properties ["useARA"].toString());

        for (int i = 0; i < (int) PluginWindow::Type::numTypes; ++i)
        {
            auto type = (PluginWindow::Type) i;

            if (node->properties.contains (PluginWindow::getOpenProp (type)))
            {
                e->setAttribute (PluginWindow::getLastXProp (type), node->properties[PluginWindow::getLastXProp (type)].toString());
                e->setAttribute (PluginWindow::getLastYProp (type), node->properties[PluginWindow::getLastYProp (type)].toString());
                e->setAttribute (PluginWindow::getOpenProp (type),  node->properties[PluginWindow::getOpenProp (type)].toString());
            }
        }

        {
            PluginDescription pd;
            plugin->fillInPluginDescription (pd);
            e->addChildElement (pd.createXml().release());
        }

        {
            MemoryBlock m;
            node->getProcessor()->getStateInformation (m);
            e->createNewChildElement ("STATE")->addTextElement (m.toBase64Encoding());
        }

        auto layout = plugin->getBusesLayout();

        auto layouts = e->createNewChildElement ("LAYOUT");
        layouts->addChildElement (createBusLayoutXml (layout, true));
        layouts->addChildElement (createBusLayoutXml (layout, false));

        return e;
    }

    jassertfalse;
    return nullptr;
}

void PluginGraph::createNodeFromXml (const XmlElement& xml)
{
    PluginDescriptionAndPreference pd;
    const auto nodeUsesARA = xml.getBoolAttribute ("useARA");

    for (auto* e : xml.getChildIterator())
    {
        if (pd.pluginDescription.loadFromXml (*e))
        {
            pd.useARA = nodeUsesARA ? PluginDescriptionAndPreference::UseARA::yes
                                    : PluginDescriptionAndPreference::UseARA::no;
            break;
        }
    }

    auto createInstanceWithFallback = [&]() -> std::unique_ptr<AudioPluginInstance>
    {
        auto createInstance = [this] (const PluginDescriptionAndPreference& description) -> std::unique_ptr<AudioPluginInstance>
        {
            String errorMessage;

            auto localDpiDisabler = makeDPIAwarenessDisablerForPlugin (description.pluginDescription);

            auto instance = formatManager.createPluginInstance (description.pluginDescription,
                                                                graph.getSampleRate(),
                                                                graph.getBlockSize(),
                                                                errorMessage);

           #if JUCE_PLUGINHOST_ARA && (JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX)
            if (instance
                && description.useARA == PluginDescriptionAndPreference::UseARA::yes
                && description.pluginDescription.hasARAExtension)
            {
                return std::make_unique<ARAPluginInstanceWrapper> (std::move (instance));
            }
           #endif

            return instance;
        };

        if (auto instance = createInstance (pd))
            return instance;

        const auto allFormats = formatManager.getFormats();
        const auto matchingFormat = std::find_if (allFormats.begin(), allFormats.end(),
                                                  [&] (const AudioPluginFormat* f) { return f->getName() == pd.pluginDescription.pluginFormatName; });

        if (matchingFormat == allFormats.end())
            return nullptr;

        const auto plugins = knownPlugins.getTypesForFormat (**matchingFormat);
        const auto matchingPlugin = std::find_if (plugins.begin(), plugins.end(),
                                                  [&] (const PluginDescription& desc) { return pd.pluginDescription.uniqueId == desc.uniqueId; });

        if (matchingPlugin == plugins.end())
            return nullptr;

        return createInstance (PluginDescriptionAndPreference { *matchingPlugin });
    };

    if (auto instance = createInstanceWithFallback())
    {
        if (auto* layoutEntity = xml.getChildByName ("LAYOUT"))
        {
            auto layout = instance->getBusesLayout();

            readBusLayoutFromXml (layout, *instance, *layoutEntity, true);
            readBusLayoutFromXml (layout, *instance, *layoutEntity, false);

            instance->setBusesLayout (layout);
        }

        if (auto node = graph.addNode (std::move (instance), NodeID ((uint32) xml.getIntAttribute ("uid"))))
        {
            if (auto* state = xml.getChildByName ("STATE"))
            {
                MemoryBlock m;
                m.fromBase64Encoding (state->getAllSubText());

                node->getProcessor()->setStateInformation (m.getData(), (int) m.getSize());
            }

            node->properties.set ("x", xml.getDoubleAttribute ("x"));
            node->properties.set ("y", xml.getDoubleAttribute ("y"));
            node->properties.set ("useARA", xml.getBoolAttribute ("useARA"));

            for (int i = 0; i < (int) PluginWindow::Type::numTypes; ++i)
            {
                auto type = (PluginWindow::Type) i;

                if (xml.hasAttribute (PluginWindow::getOpenProp (type)))
                {
                    node->properties.set (PluginWindow::getLastXProp (type), xml.getIntAttribute (PluginWindow::getLastXProp (type)));
                    node->properties.set (PluginWindow::getLastYProp (type), xml.getIntAttribute (PluginWindow::getLastYProp (type)));
                    node->properties.set (PluginWindow::getOpenProp  (type), xml.getIntAttribute (PluginWindow::getOpenProp (type)));

                    if (node->properties[PluginWindow::getOpenProp (type)])
                    {
                        jassert (node->getProcessor() != nullptr);

                        if (auto w = getOrCreateWindowFor (node, type))
                            w->toFront (true);
                    }
                }
            }
        }
    }
}

std::unique_ptr<XmlElement> PluginGraph::createXml() const
{
    auto xml = std::make_unique<XmlElement> ("FILTERGRAPH");

    for (auto* node : graph.getNodes())
        xml->addChildElement (createNodeXml (node));

    for (auto& connection : graph.getConnections())
    {
        auto e = xml->createNewChildElement ("CONNECTION");

        e->setAttribute ("srcFilter", (int) connection.source.nodeID.uid);
        e->setAttribute ("srcChannel", connection.source.channelIndex);
        e->setAttribute ("dstFilter", (int) connection.destination.nodeID.uid);
        e->setAttribute ("dstChannel", connection.destination.channelIndex);
    }

    return xml;
}

void PluginGraph::restoreFromXml (const XmlElement& xml)
{
    clear();

    for (auto* e : xml.getChildWithTagNameIterator ("FILTER"))
    {
        createNodeFromXml (*e);
        changed();
    }

    for (auto* e : xml.getChildWithTagNameIterator ("CONNECTION"))
    {
        graph.addConnection ({ { NodeID ((uint32) e->getIntAttribute ("srcFilter")), e->getIntAttribute ("srcChannel") },
                               { NodeID ((uint32) e->getIntAttribute ("dstFilter")), e->getIntAttribute ("dstChannel") } });
    }

    graph.removeIllegalConnections();
}

File PluginGraph::getDefaultGraphDocumentOnMobile()
{
    auto persistantStorageLocation = File::getSpecialLocation (File::userApplicationDataDirectory);
    return persistantStorageLocation.getChildFile ("state.filtergraph");
}
*/

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
    processorPtr->setStateInformation(output2.getData(), (int)output2.getDataSize());
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
#ifdef JUCE_WINDOWS
    auto bankFile = File::getCurrentWorkingDirectory().getFullPathName() + "\\" + String(rack.PluginName + "_Banks.txt").replace("(VST2 64bit)","");
#else
    auto bankFile = File::getSpecialLocation(File::currentExecutableFile).getFullPathName() + "../../../../../" + String(rack.PluginName + "_Banks.txt");
#endif
    
	rack.m_usesBanks = File(bankFile).exists();
	rack.m_stereoToMonoWillPhase = String(rack.PluginName).startsWith("TruePianos") || rack.PluginName == "P8";

	String errorMessage;

	InternalPluginFormat internalFormat;
	auto types = internalFormat.getAllTypes();

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
        rack.m_audioInputNode = (void*)m_audioInNode.get();
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
        for (auto j = 0U; j < (unsigned)knownPlugins.getNumTypes(); ++j)
        {
            auto name = knownPlugins.getTypes()[j].name;
            if (name.compareIgnoreCase(rack.PluginName) == 0 || name.compareIgnoreCase(String(rack.PluginName).removeCharacters(" ")) == 0 || name.compareIgnoreCase(rack.PluginName + " VSTi") == 0)
                pd = knownPlugins.getTypes()[j];
        }

		String errorMessage;
        auto processor = formatManager.createPluginInstance(pd, graph.getSampleRate(), graph.getBlockSize(), errorMessage);

        int headerSize = 0;
        String dir;
        vector<string> banks;

#ifdef JUCE_WINDOWS
        auto bankFile = File::getCurrentWorkingDirectory().getFullPathName() + "\\" + String(rack.PluginName + "_Banks.txt").replace("(VST2 64bit)", "");
#else
        auto bankFile = File::getSpecialLocation(File::currentExecutableFile).getFullPathName() + "../../../../../" + String(rack.PluginName + "_Banks.txt");
#endif
        int patches = 128;
        if (String(rack.PluginName).startsWith("JV-1080"))
        {
            headerSize = 25;
            dir = "JV-1080";
        }
        if (String(rack.PluginName).startsWith("JUPITER-8"))
        {
            headerSize = 22;
            dir = "JUPITER-8";
            patches = 64;
        }

        if (headerSize)
        {
            StringArray lines;
            File(bankFile).readLines(lines);
            for (int i = 0; i < lines.size(); ++i)
                if (lines[i] != "UNUSED")
                    banks.push_back(lines[i].toStdString());

            rack.m_overridePatches.resize(banks.size());
            for (int b = 0; b < rack.m_overridePatches.size(); ++b)
            {
                
                
            #ifdef JUCE_WINDOWS
                auto input = File(String("C:\\ProgramData\\Roland Cloud\\")+ dir +"\\" + banks [b]+".bin").createInputStream();
            #else
                auto input = File(String("/Library/Audio/Plug-Ins/Components/"+dir+".component/Contents/Resources/Patch/"+banks[b]+".bin")).createInputStream();
            #endif
                
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


            if (rack.PluginName == "M1")
            {
                auto input = File(pd.fileOrIdentifier.getCharPointer()).createInputStream()

                MemoryBlock memblock;
                input->readIntoMemoryBlock(memblock);

                const int numBanks = 37;
                int patches[numBanks] = { 100,100,100,50,50,50,50, 50,50,50,50, 50,50,50,50, 50,50,50,100,100,100,100,50,50,50,50 ,100,20,20,20,20,20,20,20,20,20,20};
                String firstPatch[numBanks];
                firstPatch[0] = "Universe  ";
                firstPatch[1] = "One World ";
                firstPatch[2] = "Stringsaw ";
                firstPatch[3] = "Squeez Me "; // patch 20
                firstPatch[4] = "Ambient   ";
                firstPatch[5] = "Cornet    "; // patch 20
                firstPatch[6] = "Piano2 Low";
                firstPatch[7] = "Rocklead  ";
                firstPatch[8] = "Mysterian ";
                firstPatch[9] = "Anvil #1  ";
                firstPatch[10] = "UniverseII";
                firstPatch[11] = "Dumbek Art";
                firstPatch[12] = "JzAltoSax ";
                firstPatch[13] = "Orient    ";
                firstPatch[14] = "Rubass    "; // patch 20
                firstPatch[15] = "FoggyRain ";
                firstPatch[16] = "NATURAL   ";
                firstPatch[17] = "ShinjukuC6";
                firstPatch[18] = "UK Strings";
                firstPatch[19] = "Ethereal  ";
                firstPatch[20] = "TT SynHorn";
                firstPatch[21] = "Water World";
                firstPatch[22] = "Init Prog ";
                firstPatch[23] = "Init Prog ";
                firstPatch[24] = "Init Prog ";
                firstPatch[25] = "Init Prog ";
                firstPatch[26] = "Aeroglide ";
                firstPatch[27] = "Live Wire "; // patch 20
                firstPatch[28] = "Natural DR";
                firstPatch[29] = "SynBells  "; // patch 20
                firstPatch[30] = "BrassHorns";
                firstPatch[31] = "Organ Perc"; // patch 20
                firstPatch[32] = "HeavySteel";
                firstPatch[33] = "Perc&Drums"; // patch 20
                firstPatch[34] = "VasePad   "; // patch 20
                firstPatch[35] = "WaterColor"; // patch 20
                firstPatch[36] = "Hoodoos   ";



                rack.m_overridePatches.resize(numBanks);

                for (int b = 0; b < numBanks; ++b)
                {
                    auto patchSize = (b==21) ? 1869 : 178;

                    char* ptr = (char*)memblock.getData();
                    char* endPtr = ptr + memblock.getSize();

                    if (firstPatch[b] == "Init Prog ")
                    {
                        for (int p = 0; p < patches[b]; ++p)
                        {
                            rack.m_overridePatches[b].push_back(firstPatch[b].trimEnd().toStdString());
                        }
                    }
                    else while (ptr < endPtr)
                    {
                        if (memcmp(ptr, firstPatch[b].getCharPointer(), 10) == 0)
                        {
                            if (b == 27 || b == 29 || b == 34 || b == 14 || b == 33 || b == 35 || b == 3 || b == 5 || b == 31)
                                ptr -= patchSize * 19;

                            for (int p = 0; p < patches[b]; ++p)
                            {
                                //*(ptr + 16) = 0; // null terminate on Mac
                                String name = ptr;

                                rack.m_overridePatches[b].push_back(name.trimEnd().toStdString());
                                ptr += patchSize;
                            }
                            break;
                        }
                        ptr++;
                    }
                }
            }

            if (rack.PluginName == "WAVESTATION")
            {
                auto input = File(pd.fileOrIdentifier.getCharPointer()).createInputStream()

                MemoryBlock memblock;
                input->readIntoMemoryBlock(memblock);

                const int numBanks = 18;
                //int patches[numBanks] = { 100,100,100,50,50,50,50, 50,50,50,50, 50,50,50,50, 50,50,50,100,100,100,100,50,50,50,50 ,100,20,20,20,20,20,20,20,20,20,20 };
                String firstPatch[numBanks];
                firstPatch[0] = "Ski Jam        ";
                firstPatch[1] = "Pharoah's Jig  ";
                firstPatch[2] = "ResoPad 1      ";
                firstPatch[3] = "Mr.Terminator\x7f ";
                firstPatch[4] = "HousePianoSplt ";
                firstPatch[5] = "Funky\x7fPlanet   ";
                firstPatch[6] = String("Toto'ly\x7f")+"6/8\x7f \x7f\x7f";
                firstPatch[7] = "Layers Of Funk ";
                firstPatch[8] = "Life\x7fGoes\x7fOn...";
                firstPatch[9] = "Mizik\x7fSplit\x7f\x7f\x7f ";
                firstPatch[10] = "The\x7fWave\x7fSong\x7f\x7f";
                firstPatch[11] = String("Piano\x7f\x7f")+"16' \x7f\x7f\x7f\x7f";
                firstPatch[12] = "Kit 1: Hall  *w";
                firstPatch[13] = "Power Monsters\x7f";
                firstPatch[14] = "Pop Man        ";
                firstPatch[15] = "Toolbox Jam !! ";
                firstPatch[16] = "Bali Dancer\x7f\x7f\x7f\x7f";
                firstPatch[17] = "TurtleBeats    ";

                rack.m_overridePatches.resize(numBanks);

                for (int b = 0; b < numBanks; ++b)
                {
                    auto patchSize = 181;

                    char* ptr = (char*)memblock.getData();
                    char* endPtr = ptr + memblock.getSize();

                    while (ptr < endPtr)
                    {
                        if (memcmp(ptr, firstPatch[b].getCharPointer(), 15) == 0)
                        {

                            for (int p = 0; p < 50; ++p)
                            {
                                //*(ptr + 16) = 0; // null terminate on Mac
                                String name = ptr;
                                name = name.replaceCharacters("\x7f", " ");
                                rack.m_overridePatches[b].push_back(name.trimEnd().toStdString());
                                ptr += patchSize;
                            }
                            break;
                        }
                        ptr++;
                    }
                }
            }


            if (rack.PluginName == "TRITON")
            {

                auto input = File(pd.fileOrIdentifier.getCharPointer()).createInputStream();


                MemoryBlock memblock;
                input->readIntoMemoryBlock(memblock);

                int bankRemap[] = { 0,1,2,3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,8,9,10,11,12,13,14,15,16 };
                const int numBanks = sizeof(bankRemap) / sizeof(bankRemap[0]);

                String firstPatch[numBanks];
                firstPatch[0] = "Noisy Stabber    ";
                firstPatch[1] = "Synth Sweeper    ";
                firstPatch[2] = "Techno Phonic    ";
                firstPatch[3] = "Ribbon Morpher   ";
                firstPatch[16] = "Stereo Grand 1+2";
                firstPatch[17] = "Mmmmoooaaah Vib.";
                firstPatch[18] = "Groove00:114-Hip";
                firstPatch[19] = "Falling Stars   ";
                firstPatch[20] = "Synth Brass     ";
                firstPatch[21] = "Marcato Str Orch";
                firstPatch[22] = "EXP Strings Vib ";
                firstPatch[23] = "C. Grand Piano  ";
                firstPatch[24] = "MassiveKillerPad";

#ifdef JUCE_WINDOWS
                auto patchSize = 22;
#else
                auto patchSize = 540;
#endif

                rack.m_overridePatches.resize(numBanks);

                for (int b = 0; b < numBanks; ++b)
                {
                    if (bankRemap[b] == -1)
                        continue;
                    char* ptr = (char*)memblock.getData();
                    char* endPtr = ptr + memblock.getSize();

                    while (ptr < endPtr)
                    {
                        if (memcmp(ptr, firstPatch[b].getCharPointer(), 16) == 0)
                        {
                            for (int p = 0; p < 128; ++p)
                            {
                                *(ptr + 16) = 0; // null terminate on Mac
                                String name = ptr;

                                rack.m_overridePatches[bankRemap[b]].push_back(name.trimEnd().toStdString());
                                ptr += patchSize;
                            }
                            break;
                        }
                        ptr++;
                    }
                }
            }


                for (int i = 0; i < rack.m_overridePatches.size(); ++i)
                {
                    if (rack.m_overridePatches[i].size() > 0)
                    {
                        File file(File::getCurrentWorkingDirectory().getFullPathName() + "\\PresetNames\\" + String(rack.PluginName).replace("(VST2 64bit)", "") + "\\" + String::formatted("%03d.txt", i));
                        file.createOutputStream();
                        for (int j = 0; j < rack.m_overridePatches[i].size(); ++j)

                        {
                            StringArray lines;
                            file.appendText(rack.m_overridePatches[i][j]);
                            if (j != rack.m_overridePatches[i].size() - 1)
                                file.appendText("\n");
                        }
                    }


                }

			if (rack.InitialState.size())
				SendChunkString(processorPtr, rack.InitialState);
			
			AddRack(processor, rack);
        }
    }
    m_keylabReady = true;
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

    if (m_performer.Root.SetLists.SetList.size() == 0 || m_performer.Root.Songs.Song.size() == 0)
        return;

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


void PluginGraph::Filter(int samples, int sampleRate, MidiBuffer &midiBuffer)
{
    if (m_keylabNeedsSettingup)
    {
        m_keylabNeedsSettingup = false;
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
                if (midi_message.getControllerNumber() == 74) // 74 = Frequency Cutoff (standard MIDI)
                    midi_message = MidiMessage::controllerEvent(midi_message.getChannel(), 16, midi_message.getControllerValue()); // Filter

                else if (midi_message.getControllerNumber() == 30)
                {
                    if (midi_message.getControllerValue() == 17)
                        midi_message = MidiMessage::controllerEvent(midi_message.getChannel(), 30, 127); // power off
                    else if (midi_message.getControllerValue() == 15)
                        midi_message = MidiMessage::controllerEvent(midi_message.getChannel(), 31, 127); // reset device
                    else
                        continue;
                }
            }

            if (midi_message.isControllerOfType(30)) // power off
            {
                if (midi_message.getControllerValue() == 127)
                {
                    m_shutdownPressCount++;
                    if (m_shutdownPressCount == 1)
                        PrintLCDScreen(output, sample_number, "Are you sure?", " ");
                    else
                    {
                        PrintLCDScreen(output, sample_number, "Shutting Down", " ");
                        system("shutdown /t 0 /s");
                    }
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


            // 4 broken wheels
            // 
            //else if (midi_message.isPitchWheel())
            //{
            //  //ignore pitch bend
            //}
            //else if (midi_message.isControllerOfType(0x01))
            //{
            //  //ignore modulation
            //}

            if (midi_message.isControllerOfType(85))
            {
				((AudioProcessorGraph::Node *)(m_masterGainNode))->getProcessor()->getParameters()[0]->setValue((((float)midi_message.getControllerValue()) / 127.f) * 0.25f);
                m_onMasterVolume(midi_message.getControllerValue());
            }
            else if (midi_message.isProgramChange())
            {
                m_performer.m_currentPerformanceIndex = midi_message.getProgramChangeNumber();
                m_pendingPerformanceIndex = m_performer.m_currentPerformanceIndex;

                UpdateCurrentRouting();
            }
            else if (midi_message.isControllerOfType(28)) // backward
            {
                if (midi_message.getControllerValue() > 0)
                {
                    m_performer.m_currentPerformanceIndex--;
                    m_pendingPerformanceIndex = m_performer.m_currentPerformanceIndex;
                    UpdateCurrentRouting();
                    UpdateLCDScreen(output, sample_number, m_performer.m_currentPerformanceIndex);
                }
                else
                    UpdateLCDScreen(output, sample_number, m_performer.m_currentPerformanceIndex); // just redraw

            }
            else if (midi_message.isControllerOfType(29)) // forward
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

String GetMachineName()
{
    return SystemStats::getComputerName();
}

void PluginGraph::SetMidiOutputDeviceName(String name)
{
    m_isKeylab88MkII = name.contains("KeyLab mkII 88");
    if (name == "" && GetMachineName() == "KEYLAB88MKII") //reset the PC if USB cable wasn't connected
        system("shutdown /t 120 /s");
}
