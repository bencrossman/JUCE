/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.5

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

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
                      public TextEditor::Listener
{
public:
    //==============================================================================
    RackTitleBar ();
    ~RackTitleBar();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void Assign(Song *song, PerformanceType *performance);
    void textEditorTextChanged(TextEditor&) override;
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    PerformanceType * m_current;
    Song * m_currentSong;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<Label> m_transposeColumn;
    std::unique_ptr<Label> m_bankProgramColumn;
    std::unique_ptr<Label> m_rangeColumn;
    std::unique_ptr<Label> m_volumeColumn;
    std::unique_ptr<Label> m_performanceLabel;
    std::unique_ptr<TextEditor> m_performanceName;
    std::unique_ptr<Label> m_songLabel;
    std::unique_ptr<TextEditor> m_songName;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RackTitleBar)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

