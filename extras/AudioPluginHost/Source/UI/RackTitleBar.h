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
class PerformanceType;
class Song;
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class RackTitleBar  : public Component,
                      public TextEditor::Listener,
                      public juce::Button::Listener,
                      public juce::Slider::Listener
{
public:
    //==============================================================================
    RackTitleBar ();
    ~RackTitleBar() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void Assign(Song *song, PerformanceType *performance);
	std::function<void(bool)> m_onMonoChanged;
	std::function<void()> m_onNextPerformance;
	std::function<void()> m_onPrevPerformance;
	std::function<void()> m_onSavePerformance;
	std::function<void(int)> m_onSetTempo;
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;
    void sliderValueChanged (juce::Slider* sliderThatWasMoved) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    PerformanceType * m_current;
    Song * m_currentSong;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::Label> m_transposeColumn;
    std::unique_ptr<juce::Label> m_bankProgramColumn;
    std::unique_ptr<juce::Label> m_rangeColumn;
    std::unique_ptr<juce::Label> m_volumeColumn;
    std::unique_ptr<juce::Label> m_performanceLabel;
    std::unique_ptr<juce::TextEditor> m_performanceName;
    std::unique_ptr<juce::Label> m_songLabel;
    std::unique_ptr<juce::TextEditor> m_songName;
    std::unique_ptr<juce::ToggleButton> m_mono;
    std::unique_ptr<juce::TextButton> m_prevPerformance;
    std::unique_ptr<juce::TextButton> m_nextPerformance;
    std::unique_ptr<juce::TextButton> m_savePerformance;
    std::unique_ptr<juce::Label> m_tempoLabel;
    std::unique_ptr<juce::Slider> m_tempo;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RackTitleBar)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

