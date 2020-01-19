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

#pragma once

#include "../UI/PluginWindow.h"
#include "../Performer.h"
#include "InternalPlugins.h"


class NonSysexFilter : public MidiFilterCallback
{
    void Filter(int samples, int sampleRate, MidiBuffer &midiBuffer) override;
};




//==============================================================================
/**
    A collection of plugins and some connections between them.
*/
class PluginGraph   : public FileBasedDocument,
                      public AudioProcessorListener,
                      private ChangeListener,
                      public MidiFilterCallback
{
public:
    //==============================================================================
    PluginGraph (AudioPluginFormatManager&, KnownPluginList& kpl);
    ~PluginGraph() override;

    //==============================================================================
    using NodeID = AudioProcessorGraph::NodeID;

    void addPlugin (const PluginDescription&, Point<double>);

    //==============================================================================
    void clear();

    PluginWindow* getOrCreateWindowFor (AudioProcessorGraph::Node*, PluginWindow::Type);
    bool closeAnyOpenPluginWindows();

    //==============================================================================
    void audioProcessorParameterChanged (AudioProcessor*, int, float) override {}
    void audioProcessorChanged (AudioProcessor*) override { changed(); }

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

    void Import(const char *filename);
    Performer *GetPerformer() { return &m_performer; }
    void Filter(int samples, int sampleRate, MidiBuffer &midiBuffer) override;
    void PrintLCDScreen(MidiBuffer &output, int sample_number, const char *text1, const char *text2);
    void UpdateLCDScreen(MidiBuffer &output, int sample_number);
    void UpdateCurrentRouting();
    void LoadSet(int setIndex);
    void SetupKeylab(MidiBuffer &output, int sample_number);
    void CreateDefaultNodes();
    void SendChunkString(AudioPluginInstance *processorPtr, StringRef str);

    //==============================================================================
    AudioProcessorGraph graph;

private:
    //==============================================================================
    AudioPluginFormatManager& formatManager;
    KnownPluginList& knownPluginList;
    OwnedArray<PluginWindow> activePluginWindows;

    Performer m_performer;
    string m_performerFilename;
    AudioProcessorGraph::Node::Ptr m_midiInNode;
    AudioProcessorGraph::Node::Ptr m_midiOutNode;
    AudioProcessorGraph::Node::Ptr m_audioOutNode;
    AudioProcessorGraph::Node::Ptr m_midiControlNode;
    AudioProcessorGraph::Node::Ptr m_masterGainNode;
    AudioProcessorGraph::Node::Ptr m_midiSysexNode;

    int       m_shutdownPressCount;
    NonSysexFilter m_nonSysexFilter;


    NodeID lastUID;
    NodeID getNextUID() noexcept;


    void addPluginCallback (std::unique_ptr<AudioPluginInstance>, const String& error, Point<double>);
    void changeListenerCallback (ChangeBroadcaster*) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginGraph)
};
