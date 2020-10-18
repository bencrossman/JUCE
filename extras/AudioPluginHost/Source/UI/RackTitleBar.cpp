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

//[Headers] You can add your own extra header files here...
#include "../Performer.h"
//[/Headers]

#include "RackTitleBar.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
RackTitleBar::RackTitleBar ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    m_transposeColumn.reset (new juce::Label (juce::String(),
                                              TRANS("Transpose")));
    addAndMakeVisible (m_transposeColumn.get());
    m_transposeColumn->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    m_transposeColumn->setJustificationType (juce::Justification::centred);
    m_transposeColumn->setEditable (false, false, false);
    m_transposeColumn->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    m_transposeColumn->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    m_transposeColumn->setBounds (628, 49, 72, 24);

    m_bankProgramColumn.reset (new juce::Label (juce::String(),
                                                TRANS("Bank/Program")));
    addAndMakeVisible (m_bankProgramColumn.get());
    m_bankProgramColumn->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    m_bankProgramColumn->setJustificationType (juce::Justification::centred);
    m_bankProgramColumn->setEditable (false, false, false);
    m_bankProgramColumn->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    m_bankProgramColumn->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    m_bankProgramColumn->setBounds (233, 49, 150, 24);

    m_rangeColumn.reset (new juce::Label (juce::String(),
                                          TRANS("Range")));
    addAndMakeVisible (m_rangeColumn.get());
    m_rangeColumn->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    m_rangeColumn->setJustificationType (juce::Justification::centred);
    m_rangeColumn->setEditable (false, false, false);
    m_rangeColumn->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    m_rangeColumn->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    m_rangeColumn->setBounds (724, 49, 64, 24);

    m_volumeColumn.reset (new juce::Label (juce::String(),
                                           TRANS("Volume")));
    addAndMakeVisible (m_volumeColumn.get());
    m_volumeColumn->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    m_volumeColumn->setJustificationType (juce::Justification::centred);
    m_volumeColumn->setEditable (false, false, false);
    m_volumeColumn->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    m_volumeColumn->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    m_volumeColumn->setBounds (96, 49, 128, 24);

    m_performanceLabel.reset (new juce::Label (juce::String(),
                                               TRANS("Performance")));
    addAndMakeVisible (m_performanceLabel.get());
    m_performanceLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    m_performanceLabel->setJustificationType (juce::Justification::centredLeft);
    m_performanceLabel->setEditable (false, false, false);
    m_performanceLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    m_performanceLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    m_performanceLabel->setBounds (0, 25, 150, 24);

    m_performanceName.reset (new juce::TextEditor (juce::String()));
    addAndMakeVisible (m_performanceName.get());
    m_performanceName->setMultiLine (false);
    m_performanceName->setReturnKeyStartsNewLine (false);
    m_performanceName->setReadOnly (true);
    m_performanceName->setScrollbarsShown (false);
    m_performanceName->setCaretVisible (false);
    m_performanceName->setPopupMenuEnabled (true);
    m_performanceName->setText (juce::String());

    m_performanceName->setBounds (136, 26, 272, 22);

    m_songLabel.reset (new juce::Label (juce::String(),
                                        TRANS("Song")));
    addAndMakeVisible (m_songLabel.get());
    m_songLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    m_songLabel->setJustificationType (juce::Justification::centredLeft);
    m_songLabel->setEditable (false, false, false);
    m_songLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    m_songLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    m_songLabel->setBounds (0, 1, 150, 24);

    m_songName.reset (new juce::TextEditor (juce::String()));
    addAndMakeVisible (m_songName.get());
    m_songName->setMultiLine (false);
    m_songName->setReturnKeyStartsNewLine (false);
    m_songName->setReadOnly (true);
    m_songName->setScrollbarsShown (false);
    m_songName->setCaretVisible (false);
    m_songName->setPopupMenuEnabled (true);
    m_songName->setText (juce::String());

    m_songName->setBounds (136, 2, 272, 22);

    m_mono.reset (new juce::ToggleButton (juce::String()));
    addAndMakeVisible (m_mono.get());
    m_mono->setButtonText (TRANS("Mono"));
    m_mono->addListener (this);

    m_mono->setBounds (744, 0, 80, 24);

    m_prevPerformance.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (m_prevPerformance.get());
    m_prevPerformance->setButtonText (TRANS("<"));
    m_prevPerformance->addListener (this);

    m_prevPerformance->setBounds (416, 24, 23, 24);

    m_nextPerformance.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (m_nextPerformance.get());
    m_nextPerformance->setButtonText (TRANS(">"));
    m_nextPerformance->addListener (this);

    m_nextPerformance->setBounds (448, 24, 23, 24);

    m_savePerformance.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (m_savePerformance.get());
    m_savePerformance->setButtonText (TRANS("Save performance"));
    m_savePerformance->addListener (this);

    m_savePerformance->setBounds (480, 24, 150, 24);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (816, 70);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

RackTitleBar::~RackTitleBar()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    m_transposeColumn = nullptr;
    m_bankProgramColumn = nullptr;
    m_rangeColumn = nullptr;
    m_volumeColumn = nullptr;
    m_performanceLabel = nullptr;
    m_performanceName = nullptr;
    m_songLabel = nullptr;
    m_songName = nullptr;
    m_mono = nullptr;
    m_prevPerformance = nullptr;
    m_nextPerformance = nullptr;
    m_savePerformance = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void RackTitleBar::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void RackTitleBar::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void RackTitleBar::buttonClicked (juce::Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == m_mono.get())
    {
        //[UserButtonCode_m_mono] -- add your button handler code here..
		m_onMonoChanged(m_mono->getToggleState());
        //[/UserButtonCode_m_mono]
    }
    else if (buttonThatWasClicked == m_prevPerformance.get())
    {
        //[UserButtonCode_m_prevPerformance] -- add your button handler code here..
		m_onPrevPerformance();
        //[/UserButtonCode_m_prevPerformance]
    }
    else if (buttonThatWasClicked == m_nextPerformance.get())
    {
        //[UserButtonCode_m_nextPerformance] -- add your button handler code here..
		m_onNextPerformance();
        //[/UserButtonCode_m_nextPerformance]
    }
    else if (buttonThatWasClicked == m_savePerformance.get())
    {
        //[UserButtonCode_m_savePerformance] -- add your button handler code here..
		m_onSavePerformance();
        //[/UserButtonCode_m_savePerformance]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void RackTitleBar::Assign(Song *song, PerformanceType *performance)
{
    m_performanceName->setText(performance->Name, false);
    m_songName->setText(song->Name, false);

}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="RackTitleBar" componentName=""
                 parentClasses="public Component, public TextEditor::Listener"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="816"
                 initialHeight="70">
  <BACKGROUND backgroundColour="ff323e44"/>
  <LABEL name="" id="df98b892f707f559" memberName="m_transposeColumn"
         virtualName="" explicitFocusOrder="0" pos="628 49 72 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Transpose" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="1ec7c2bf1356976c" memberName="m_bankProgramColumn"
         virtualName="" explicitFocusOrder="0" pos="233 49 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Bank/Program" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="45ccc111fb3f525b" memberName="m_rangeColumn" virtualName=""
         explicitFocusOrder="0" pos="724 49 64 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Range" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="7f6211731f4b9927" memberName="m_volumeColumn" virtualName=""
         explicitFocusOrder="0" pos="96 49 128 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Volume" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="7c8e939d79f153c5" memberName="m_performanceLabel"
         virtualName="" explicitFocusOrder="0" pos="0 25 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Performance" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <TEXTEDITOR name="" id="fd21b0d2d8107be1" memberName="m_performanceName"
              virtualName="" explicitFocusOrder="0" pos="136 26 272 22" initialText=""
              multiline="0" retKeyStartsLine="0" readonly="1" scrollbars="0"
              caret="0" popupmenu="1"/>
  <LABEL name="" id="ab64af59d8be5fe3" memberName="m_songLabel" virtualName=""
         explicitFocusOrder="0" pos="0 1 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Song" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <TEXTEDITOR name="" id="21ef937502d210df" memberName="m_songName" virtualName=""
              explicitFocusOrder="0" pos="136 2 272 22" initialText="" multiline="0"
              retKeyStartsLine="0" readonly="1" scrollbars="0" caret="0" popupmenu="1"/>
  <TOGGLEBUTTON name="" id="e7ecdde1129e3bb5" memberName="m_mono" virtualName=""
                explicitFocusOrder="0" pos="744 0 80 24" buttonText="Mono" connectedEdges="0"
                needsCallback="1" radioGroupId="0" state="0"/>
  <TEXTBUTTON name="" id="cf6b4fb03023ba4f" memberName="m_prevPerformance"
              virtualName="" explicitFocusOrder="0" pos="416 24 23 24" buttonText="&lt;"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="af469b19337acdca" memberName="m_nextPerformance"
              virtualName="" explicitFocusOrder="0" pos="448 24 23 24" buttonText="&gt;"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="d8ed5c4e2ab3cd09" memberName="m_savePerformance"
              virtualName="" explicitFocusOrder="0" pos="480 24 150 24" buttonText="Save performance"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

