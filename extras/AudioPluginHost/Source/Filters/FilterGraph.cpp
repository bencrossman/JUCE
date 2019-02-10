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
#include "FilterGraph.h"
#include "InternalFilters.h"
#include "../UI/GraphEditorPanel.h"
#include <windows.h>

//==============================================================================
FilterGraph::FilterGraph (AudioPluginFormatManager& fm, KnownPluginList& kpl)
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

FilterGraph::~FilterGraph()
{
    graph.removeListener (this);
    graph.removeChangeListener (this);
    graph.clear();
}

FilterGraph::NodeID FilterGraph::getNextUID() noexcept
{
    return FilterGraph::NodeID (++(lastUID.uid));
}

//==============================================================================
void FilterGraph::changeListenerCallback (ChangeBroadcaster*)
{
    changed();

    for (int i = activePluginWindows.size(); --i >= 0;)
        if (! graph.getNodes().contains (activePluginWindows.getUnchecked(i)->node))
            activePluginWindows.remove (i);
}

void FilterGraph::addPlugin (const PluginDescription& desc, Point<double> p)
{
    struct AsyncCallback : public AudioPluginFormat::InstantiationCompletionCallback
    {
        AsyncCallback (FilterGraph& g, Point<double> pos)  : owner (g), position (pos)
        {}

        void completionCallback (AudioPluginInstance* instance, const String& error) override
        {
            owner.addFilterCallback (instance, error, position);
        }

        FilterGraph& owner;
        Point<double> position;
    };

    formatManager.createPluginInstanceAsync (desc,
                                             graph.getSampleRate(),
                                             graph.getBlockSize(),
                                             new AsyncCallback (*this, p));
}

void FilterGraph::addFilterCallback (AudioPluginInstance* instance, const String& error, Point<double> pos)
{
    if (instance == nullptr)
    {
        AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                          TRANS("Couldn't create filter"),
                                          error);
    }
    else
    {
        instance->enableAllBuses();

        if (auto node = graph.addNode (instance))
        {
            node->properties.set ("x", pos.x);
            node->properties.set ("y", pos.y);
            changed();
        }
    }
}


//==============================================================================
void FilterGraph::clear()
{
    closeAnyOpenPluginWindows();
    graph.clear();
    changed();
}

PluginWindow* FilterGraph::getOrCreateWindowFor (AudioProcessorGraph::Node* node, PluginWindow::Type type)
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

bool FilterGraph::closeAnyOpenPluginWindows()
{
    bool wasEmpty = activePluginWindows.isEmpty();
    activePluginWindows.clear();
    return ! wasEmpty;
}

//==============================================================================
String FilterGraph::getDocumentTitle()
{
    if (! getFile().exists())
        return "Unnamed";

    return getFile().getFileNameWithoutExtension();
}

void FilterGraph::newDocument()
{
    clear();
    setFile ({});

    graph.removeChangeListener (this);

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

    MessageManager::callAsync ([this] () {
        setChangedFlag (false);
        graph.addChangeListener (this);
    } );
}

Result FilterGraph::loadDocument (const File& file)
{
    clear();

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

    graph.removeChangeListener (this);

    m_performerFilename = file.getFileNameWithoutExtension().toStdString();
    XmlArchive::Load(file.getFullPathName().getCharPointer(), m_performer);
    m_performer.ResolveIDs();

    if (m_performer.Root.Racks.Rack.size() == 0)
        return Result::fail("No racks");

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
            }
        }

        if (auto* instance = formatManager.createPluginInstance(pd, graph.getSampleRate(), graph.getBlockSize(), errorMessage))
        {
            auto node = graph.addNode(instance, (NodeID)rack.ID);

            auto midi = graph.addNode(formatManager.createPluginInstance(internalFormat.midiFilterDesc, graph.getSampleRate(), graph.getBlockSize(), errorMessage));

            auto gain = graph.addNode(formatManager.createPluginInstance(internalFormat.gainDesc, graph.getSampleRate(), graph.getBlockSize(), errorMessage));

            // State stuff for later
            //MemoryBlock m;
            //m.fromBase64Encoding(char*);
            //node->getProcessor()->setStateInformation(m.getData(), (int)m.getSize()); 

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

    changed();

    MessageManager::callAsync ([this] () {
        setChangedFlag (false);
        graph.addChangeListener (this);
    } );

    return Result::ok();
}

Result FilterGraph::saveDocument (const File& file)
{
    XmlArchive::Save(file.getFullPathName().getCharPointer(), m_performer);
    return Result::ok();
}

File FilterGraph::getLastDocumentOpened()
{
    RecentlyOpenedFilesList recentFiles;
    recentFiles.restoreFromString (getAppProperties().getUserSettings()
                                        ->getValue ("recentFilterGraphFiles"));

    return recentFiles.getFile (0);
}

void FilterGraph::setLastDocumentOpened (const File& file)
{
    RecentlyOpenedFilesList recentFiles;
    recentFiles.restoreFromString (getAppProperties().getUserSettings()
                                        ->getValue ("recentFilterGraphFiles"));

    recentFiles.addFile (file);

    getAppProperties().getUserSettings()
        ->setValue ("recentFilterGraphFiles", recentFiles.toString());
}

void FilterGraph::Import(const char *filename)
{
    m_performer.Import(filename);
}


int m_currentSong = 0;
int m_pendingSong = 0;
int m_pendingSet = 0;

void FilterGraph::SetupKeylab(MidiBuffer &output, int sample_number)
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


void FilterGraph::PrintLCDScreen(MidiBuffer &output, int sample_number, const char *text1, const char *text2)
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
    memcpy(mes + meslen, text1, strlen(text1) + 1); meslen += strlen(text1) + 1;
    mes[meslen++] = 0x02;
    memcpy(mes + meslen, text2, strlen(text2) + 1); meslen += strlen(text2) + 1;

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

void FilterGraph::UpdateLCDScreen(MidiBuffer &output, int sample_number)
{
    string songName1, songName2;
    songName1 = m_performer.Root.Songs.Song[m_currentSong].Name;
    songName2 = m_performer.Root.Songs.Song[m_currentSong].PerformancePtr[0]->Name; // TODO some sort of index into songs & performances

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

void FilterGraph::UpdateCurrentRouting()
{
    // this code is in GraphEditorPanel at the moment
}

void FilterGraph::LoadSet(int setIndex)
{
    setIndex;
}

bool m_keylabNeedsSettingup = true;

void FilterGraph::Filter(int samples, int sampleRate, MidiBuffer &midiBuffer)
{
    if (m_keylabNeedsSettingup)
    {
        m_keylabNeedsSettingup = false;
        SetupKeylab(midiBuffer, 0);
    }
    samples;
    sampleRate;
    if (!midiBuffer.isEmpty())
    {
        MidiMessage midi_message(0xf0);
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
                if (m_shutdownPressCount>1)
                {
                    PrintLCDScreen(output, sample_number, "Shutting Down", " ");
                    system("shutdown /t 0 /s");
                }
            }
            else
            {
                if (m_shutdownPressCount) // to remove confirm text
                {
                    if (m_performer.Root.SetLists.SetList.size()>0)
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
                if (m_performer.Root.SetLists.SetList.size())
                {
                    m_currentSong = midi_message.getProgramChangeNumber() % (int)m_performer.Root.SetLists.SetList.size();
                    m_pendingSong = m_currentSong;
                }

                UpdateCurrentRouting();
            }
            else if (midi_message.isControllerOfType(111)) // backward
            {
                if (midi_message.getControllerValue()>0)
                {
                    m_currentSong--;
                    if (m_currentSong<0)
                        m_currentSong = m_performer.Root.SetLists.SetList.size() - 1;
                    if (m_currentSong<0)
                        m_currentSong = 0;
                    m_pendingSong = m_currentSong;
                    UpdateCurrentRouting();
                }
                else
                    UpdateLCDScreen(output, sample_number);

            }
            else if (midi_message.isControllerOfType(116)) // forward
            {
                if (midi_message.getControllerValue()>0)
                {
                    m_currentSong++;
                    if (m_currentSong >= (int)m_performer.Root.SetLists.SetList.size())
                        m_currentSong = 0;
                    m_pendingSong = m_currentSong;
                    UpdateCurrentRouting();
                }
                else
                    UpdateLCDScreen(output, sample_number);

            }
            else if (midi_message.isControllerOfType(115))
            {
                if (midi_message.getControllerValue()>0)
                {
                    m_currentSong = m_pendingSong;
                    UpdateCurrentRouting();
                }
                else
                    UpdateLCDScreen(output, sample_number); // just redraw
            }
            else if (midi_message.isControllerOfType(114) && midi_message.getControllerValue() == 0x3f) // anticlockwise
            {
                m_pendingSong--;
                if (m_pendingSong<0)
                    m_pendingSong = m_performer.Root.SetLists.SetList.size() - 1;
                if (m_pendingSong<0)
                    m_pendingSong = 0;
                UpdateLCDScreen(output, sample_number);
            }
            else if (midi_message.isControllerOfType(114) && midi_message.getControllerValue() == 0x41) // clockwise
            {
                m_pendingSong++;
                if (m_pendingSong >= (int)m_performer.Root.SetLists.SetList.size())
                    m_pendingSong = 0;
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
                if (m_pendingSet<0)
                    m_pendingSet = m_performer.Root.SetLists.SetList.size() - 1;
                if (m_pendingSet<0)
                    m_pendingSet = 0;
                if (m_performer.Root.SetLists.SetList.size()>0)
                    PrintLCDScreen(output, sample_number, m_performerFilename.c_str(), m_performer.Root.SetLists.SetList[m_pendingSet].Name.c_str());
            }
            else if (midi_message.isControllerOfType(112) && midi_message.getControllerValue() == 0x41) // category clockwise
            {
                m_pendingSet++;
                if (m_pendingSet >= (int)m_performer.Root.SetLists.SetList.size())
                    m_pendingSet = 0;
                if (m_performer.Root.SetLists.SetList.size()>0)
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
        MidiMessage midi_message(0xf0);
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


