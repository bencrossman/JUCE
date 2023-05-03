/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2022 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 7 End-User License
   Agreement and JUCE Privacy Policy.

   End User License Agreement: www.juce.com/juce-7-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

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

    //==============================================================================
    void clear();

    PluginWindow* getOrCreateWindowFor (AudioProcessorGraph::Node*, PluginWindow::Type);
    bool closeAnyOpenPluginWindows();

    //==============================================================================
    void audioProcessorParameterChanged (AudioProcessor*, int, float) override {}
    void audioProcessorChanged (AudioProcessor*, const ChangeDetails&) override { changed(); }

    //==============================================================================

    static const char* getFilenameSuffix()      { return ".performer"; }
    static const char* getFilenameWildcard()    { return "*.performer"; }

    //==============================================================================
    void newDocument();
    String getDocumentTitle() override;
    Result loadDocument (const File& file) override;
    Result saveDocument (const File& file) override;
    File getLastDocumentOpened() override;
    void setLastDocumentOpened (const File& file) override;
    void setupPerformer();
	void AddRack(std::unique_ptr<AudioPluginInstance> &processorPtr, Device &rack);

    void Import(const char *filename);
    Performer *GetPerformer() { return &m_performer; }
    void Filter(int samples, int sampleRate, MidiBuffer &midiBuffer) override;
    void PrintLCDScreen(MidiBuffer &output, int sample_number, const char *text1, const char *text2);
    void UpdateLCDScreen(MidiBuffer &output, int sample_number, int index);
    void UpdateCurrentRouting();
    void LoadSet(int setIndex);
    void SetupKeylab(MidiBuffer &output, int sample_number);
    void CreateDefaultNodes();
    void SendChunkString(AudioPluginInstance *processorPtr, StringRef str);

    //==============================================================================
    AudioProcessorGraph graph;
    std::function<void()> m_onProgramChange = NULL;
	void SetTempo(double tempo);
	void SetMono(bool mono);
	bool IsMono() { return m_mono; }
    void PrevPerformance() { m_manualMidi = m_isKeylab88MkII ? 28: 111; }
    void NextPerformance() { m_manualMidi = m_isKeylab88MkII ? 29 : 116; }
    void SetMidiOutputDeviceName(String name) { m_isKeylab88MkII = name.contains("KeyLab mkII 88");}

private:
    //==============================================================================
    AudioPluginFormatManager& formatManager;
    KnownPluginList& knownPlugins;
    OwnedArray<PluginWindow> activePluginWindows;
    ScopedMessageBox messageBox;

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


    void addPluginCallback (std::unique_ptr<AudioPluginInstance>,
                            const String& error,
                            Point<double>,
                            PluginDescriptionAndPreference::UseARA useARA);
    void changeListenerCallback (ChangeBroadcaster*) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginGraph)
};
