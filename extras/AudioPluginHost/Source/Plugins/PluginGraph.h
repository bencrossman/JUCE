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

#pragma once

#include "../UI/PluginWindow.h"
#include "../Performer.h"
#include "InternalPlugins.h"


class NonSysexFilter : public MidiFilterCallback
{
    void Filter(int samples, int sampleRate, MidiBuffer &midiBuffer) override;
};

extern int g_engineDuck;

//==============================================================================
/** A type that encapsulates a PluginDescription and some preferences regarding
    how plugins of that description should be instantiated.
*/
struct PluginDescriptionAndPreference
{
    enum class UseARA { no, yes };

    PluginDescriptionAndPreference() = default;

    explicit PluginDescriptionAndPreference (PluginDescription pd)
        : pluginDescription (std::move (pd)),
          useARA (pluginDescription.hasARAExtension ? PluginDescriptionAndPreference::UseARA::yes
                                                    : PluginDescriptionAndPreference::UseARA::no)
    {}

    PluginDescriptionAndPreference (PluginDescription pd, UseARA ara)
        : pluginDescription (std::move (pd)), useARA (ara)
    {}

    PluginDescription pluginDescription;
    UseARA useARA = UseARA::no;
};

//==============================================================================
/**
    A collection of plugins and some connections between them.
*/
class PluginGraph final : public FileBasedDocument,
                          public AudioProcessorListener,
                          private ChangeListener,
                          public MidiFilterCallback
{
public:
    //==============================================================================
    PluginGraph (AudioPluginFormatManager&, KnownPluginList&);
    ~PluginGraph() override;

    //==============================================================================
    using NodeID = AudioProcessorGraph::NodeID;

    void addPlugin (const PluginDescriptionAndPreference&, Point<double>);
/*
    AudioProcessorGraph::Node::Ptr getNodeForName (const String& name) const;

    void setNodePosition (NodeID, Point<double>);
    Point<double> getNodePosition (NodeID) const;
*/
    //==============================================================================
    void clear();

    PluginWindow* getOrCreateWindowFor (AudioProcessorGraph::Node*, PluginWindow::Type);
/*
    void closeCurrentlyOpenWindowsFor (AudioProcessorGraph::NodeID);
*/
    bool closeAnyOpenPluginWindows();

    //==============================================================================
    void audioProcessorParameterChanged (AudioProcessor*, int, float) override {}
    void audioProcessorChanged (AudioProcessor*, const ChangeDetails&) override { changed(); }

    //==============================================================================
/*
    std::unique_ptr<XmlElement> createXml() const;
    void restoreFromXml (const XmlElement&);
*/

    static const char* getFilenameSuffix()      { return ".performer"; }
    static const char* getFilenameWildcard()    { return "*.performer"; }

    //==============================================================================
    void newDocument();
    String getDocumentTitle() override;
    Result loadDocument (const File& file) override;
    Result saveDocument (const File& file) override;
    File getLastDocumentOpened() override;
    void setLastDocumentOpened (const File& file) override;

/*
    static File getDefaultGraphDocumentOnMobile();
*/

    //==============================================================================
    AudioProcessorGraph graph;

    void setupPerformer();
	void AddRack(std::unique_ptr<AudioPluginInstance> &processorPtr, Device &rack);

    void Import(const char *filename);
    Performer *GetPerformer() { return &m_performer; }
    void Filter(int samples, int sampleRate, MidiBuffer &midiBuffer) override;
    void PrintLCDScreen(MidiBuffer &output, int sample_number, const char *text1, const char *text2);
    void UpdateLCDScreen(MidiBuffer &output, int sample_number, int index);
    void UpdateCurrentRouting();
    void LoadSet(int setIndex);
    void CreateDefaultNodes();
    void SendChunkString(AudioPluginInstance *processorPtr, StringRef str);

    std::function<void()> m_onProgramChange = NULL;
    std::function<void(int)> m_onMasterVolume = NULL;
    bool m_initializing = true;

    void SetTempo(double tempo);
	void SetMono(bool mono);
	bool IsMono() { return m_mono; }

    bool DoesPendingingPerformanceDoConsume(int offset)
    {
        auto performer = GetPerformer();
        PerformanceType* performance;
        Song* song = NULL;
        performer->GetPerformanceByIndex(performance, song, performer->m_currentPerformanceIndex + offset);
        for (int i = 0; i < performance->Zone.size(); ++i)
        {
            if (performance->Zone[i].Device->m_hasStupidCrossfade && !performance->Zone[i].Mute)
            {
                return true;
            }
        }
        return false;
    }

    void PrevPerformance()
    {
        if (!DoesPendingingPerformanceDoConsume(-1))
        {
            m_manualMidi = 28;
            return;
        }

        if (m_performanceChangePending)
            return;

        m_performanceChangePending = true;

        g_engineDuck = 3;
        Thread::launch([this]()
        {
            Thread::sleep(50);
            m_manualMidi = 28;
            Thread::sleep(50);
            g_engineDuck = 1;
            m_performanceChangePending = false;

        });
    }
    void NextPerformance() 
    { 
        if (!DoesPendingingPerformanceDoConsume(1))
        {
            m_manualMidi = 29;
            return;
        }

        if (m_performanceChangePending)
            return;

        m_performanceChangePending = true;

        g_engineDuck = 3;
        Thread::launch([this]() 
        {
            Thread::sleep(50); 
            m_manualMidi = 29;  
            Thread::sleep(50);
            g_engineDuck = 1;
            m_performanceChangePending = false;

        });
        
    }
    void SetMidiOutputDeviceName(String name);

private:
    //==============================================================================
    AudioPluginFormatManager& formatManager;
    KnownPluginList& knownPlugins;
    OwnedArray<PluginWindow> activePluginWindows;
    ScopedMessageBox messageBox;

    bool m_performanceChangePending = false;

    Performer m_performer;
    string m_performerFilename;
    AudioProcessorGraph::Node::Ptr m_midiInNode;
    AudioProcessorGraph::Node::Ptr m_midiOutNode;
    AudioProcessorGraph::Node::Ptr m_audioInNode;
    AudioProcessorGraph::Node::Ptr m_audioOutNode;
    AudioProcessorGraph::Node::Ptr m_midiControlNode;
    AudioProcessorGraph::Node::Ptr m_masterGainNode;
    AudioProcessorGraph::Node::Ptr m_midiSysexNode;


    bool m_keylabNeedsSettingup = true;
    bool m_keylabReady = false;

    int       m_shutdownPressCount;
    NonSysexFilter m_nonSysexFilter;
	bool m_mono;
    int m_manualMidi = 0;

    bool m_isKeylab88MkII = false;

    NodeID lastUID;
    NodeID getNextUID() noexcept;
/*
    void createNodeFromXml (const XmlElement&);
*/
    void addPluginCallback (std::unique_ptr<AudioPluginInstance>,
                            const String& error,
                            Point<double>,
                            PluginDescriptionAndPreference::UseARA useARA);
    void changeListenerCallback (ChangeBroadcaster*) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginGraph)
};
