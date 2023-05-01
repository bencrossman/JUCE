/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 6.0.1

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../../JuceLibraryCode/JuceHeader.h"
#include "../Plugins/InternalPlugins.h"
class Device;
class Zone;
class FilterGraph;
class GraphEditorPanel;

using namespace std;
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class RackRow  : public Component,
                 public TextEditor::Listener,
                 public MidiFilterCallback,
                 public juce::Button::Listener,
                 public juce::Slider::Listener,
                 public juce::ComboBox::Listener
{
public:
    //==============================================================================
    RackRow ();
    ~RackRow() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void UpdateKeyboard();
    void Setup(Device &device, PluginGraph &pluginGraph, GraphEditorPanel &panel);
    void Assign(Zone *zone);
    int ID() { return m_id; }
    void textEditorTextChanged(TextEditor&) override;
    void SetSoloMode(bool mode);
    bool IsSolo() { return m_solo->getToggleState(); }
    void Filter(int samples, int sampleRate, MidiBuffer &midiBuffer) override;
    static void SetTempo(float tempo) { m_tempo = tempo; }
    void handleCommandMessage(int id) override;
    enum {CommandUpdateProgramList = 1, CommandBypass=2};
    void SendPresetStateData();
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;
    void sliderValueChanged (juce::Slider* sliderThatWasMoved) override;
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override;
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    MidiKeyboardState *m_keyboardState;
    Zone *m_current;
    int m_id;
    PluginGraph* graph;
    GraphEditorPanel* panel;
    bool m_soloMode;
    bool m_pendingProgram = false;
    bool m_pendingSoundOff = false;
    bool m_pendingProgramNames = false;
    bool m_pendingBank = false;
    bool m_pendingBypass = false;
    bool m_allowCC16 = false;
    bool m_manualPatchNames = false;
    vector<int> m_notesDown;
    bool m_anyNotesDown;
    int m_arpeggiatorBeat;
    int m_lastNote;
    float m_arpeggiatorTimer;
    static float m_tempo;
    bool m_lastZoneHadOverrideState = false;
    String m_lastSendPresetStateDataFilename;
    AudioProcessorGraph::Node* m_midiFilterNode = nullptr; // didnt want to hold this but had to
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::GroupComponent> m_deviceName;
    std::unique_ptr<juce::ToggleButton> m_solo;
    std::unique_ptr<juce::ToggleButton> m_mute;
    std::unique_ptr<juce::Slider> m_volume;
    std::unique_ptr<juce::ComboBox> m_bank;
    std::unique_ptr<juce::ComboBox> m_program;
    std::unique_ptr<juce::TextEditor> m_transpose;
    std::unique_ptr<juce::Label> m_to;
    std::unique_ptr<juce::TextEditor> m_lowKey;
    std::unique_ptr<juce::TextEditor> m_highKey;
    std::unique_ptr<juce::ImageButton> m_deviceSettings;
    std::unique_ptr<MidiKeyboardComponent> m_keyboard;
    std::unique_ptr<juce::ComboBox> m_noteMode;
    std::unique_ptr<juce::Label> m_missing;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RackRow)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

