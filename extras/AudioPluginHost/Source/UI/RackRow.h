/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.3.2

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../../JuceLibraryCode/JuceHeader.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class RackRow  : public Component,
                 public Button::Listener,
                 public Slider::Listener,
                 public ComboBox::Listener
{
public:
    //==============================================================================
    RackRow ();
    ~RackRow();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void UpdateKeyboard();
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;

    // Binary resources:
    static const char* truePianos_png;
    static const int truePianos_pngSize;


private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    MidiKeyboardState *m_keyboardState;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<GroupComponent> m_deviceName;
    std::unique_ptr<ToggleButton> m_solo;
    std::unique_ptr<ToggleButton> m_mute;
    std::unique_ptr<Slider> m_volume;
    std::unique_ptr<ComboBox> m_bank;
    std::unique_ptr<ComboBox> m_program;
    std::unique_ptr<TextEditor> m_transpose;
    std::unique_ptr<Label> m_to;
    std::unique_ptr<TextEditor> m_lowKey;
    std::unique_ptr<TextEditor> m_highKey;
    std::unique_ptr<ImageButton> m_deviceSettings;
    std::unique_ptr<MidiKeyboardComponent> m_keyboard;
    std::unique_ptr<ToggleButton> m_doubleOctave;
    std::unique_ptr<ToggleButton> m_arperggiator;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RackRow)
};

//[EndFile] You can add extra defines here...
//[/EndFile]
