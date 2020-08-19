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

    m_transposeColumn.reset (new Label (String(),
                                        TRANS("Transpose")));
    addAndMakeVisible (m_transposeColumn.get());
    m_transposeColumn->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    m_transposeColumn->setJustificationType (Justification::centred);
    m_transposeColumn->setEditable (false, false, false);
    m_transposeColumn->setColour (TextEditor::textColourId, Colours::black);
    m_transposeColumn->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    m_transposeColumn->setBounds (628, 49, 72, 24);

    m_bankProgramColumn.reset (new Label (String(),
                                          TRANS("Bank/Program")));
    addAndMakeVisible (m_bankProgramColumn.get());
    m_bankProgramColumn->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    m_bankProgramColumn->setJustificationType (Justification::centred);
    m_bankProgramColumn->setEditable (false, false, false);
    m_bankProgramColumn->setColour (TextEditor::textColourId, Colours::black);
    m_bankProgramColumn->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    m_bankProgramColumn->setBounds (233, 49, 150, 24);

    m_rangeColumn.reset (new Label (String(),
                                    TRANS("Range")));
    addAndMakeVisible (m_rangeColumn.get());
    m_rangeColumn->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    m_rangeColumn->setJustificationType (Justification::centred);
    m_rangeColumn->setEditable (false, false, false);
    m_rangeColumn->setColour (TextEditor::textColourId, Colours::black);
    m_rangeColumn->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    m_rangeColumn->setBounds (724, 49, 64, 24);

    m_volumeColumn.reset (new Label (String(),
                                     TRANS("Volume")));
    addAndMakeVisible (m_volumeColumn.get());
    m_volumeColumn->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    m_volumeColumn->setJustificationType (Justification::centred);
    m_volumeColumn->setEditable (false, false, false);
    m_volumeColumn->setColour (TextEditor::textColourId, Colours::black);
    m_volumeColumn->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    m_volumeColumn->setBounds (96, 49, 128, 24);

    m_performanceLabel.reset (new Label (String(),
                                         TRANS("Performance")));
    addAndMakeVisible (m_performanceLabel.get());
    m_performanceLabel->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    m_performanceLabel->setJustificationType (Justification::centredLeft);
    m_performanceLabel->setEditable (false, false, false);
    m_performanceLabel->setColour (TextEditor::textColourId, Colours::black);
    m_performanceLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    m_performanceLabel->setBounds (0, 25, 150, 24);

    m_performanceName.reset (new TextEditor (String()));
    addAndMakeVisible (m_performanceName.get());
    m_performanceName->setMultiLine (false);
    m_performanceName->setReturnKeyStartsNewLine (false);
    m_performanceName->setReadOnly (false);
    m_performanceName->setScrollbarsShown (false);
    m_performanceName->setCaretVisible (true);
    m_performanceName->setPopupMenuEnabled (true);
    m_performanceName->setText (String());

    m_performanceName->setBounds (136, 26, 272, 22);

    m_songLabel.reset (new Label (String(),
                                  TRANS("Song")));
    addAndMakeVisible (m_songLabel.get());
    m_songLabel->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    m_songLabel->setJustificationType (Justification::centredLeft);
    m_songLabel->setEditable (false, false, false);
    m_songLabel->setColour (TextEditor::textColourId, Colours::black);
    m_songLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    m_songLabel->setBounds (0, 1, 150, 24);

    m_songName.reset (new TextEditor (String()));
    addAndMakeVisible (m_songName.get());
    m_songName->setMultiLine (false);
    m_songName->setReturnKeyStartsNewLine (false);
    m_songName->setReadOnly (false);
    m_songName->setScrollbarsShown (false);
    m_songName->setCaretVisible (true);
    m_songName->setPopupMenuEnabled (true);
    m_songName->setText (String());

    m_songName->setBounds (136, 2, 272, 22);

    m_mono.reset (new ToggleButton (String()));
    addAndMakeVisible (m_mono.get());
    m_mono->setButtonText (TRANS("Mono"));
    m_mono->addListener (this);

    m_mono->setBounds (424, 0, 80, 24);


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


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void RackTitleBar::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff323e44));

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

void RackTitleBar::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == m_mono.get())
    {
        //[UserButtonCode_m_mono] -- add your button handler code here..
		m_onMonoChanged(m_mono->getToggleState());
        //[/UserButtonCode_m_mono]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void RackTitleBar::Assign(Song *song, PerformanceType *performance)
{
    m_current = performance;
    m_currentSong = song;
    m_performanceName->setText(performance->Name, false);
    m_songName->setText(song->Name, false);

}

void RackTitleBar::textEditorTextChanged(TextEditor&)
{
    m_current->Name = m_performanceName->getText().toStdString();
    m_currentSong->Name = m_songName->getText().toStdString();
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
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="0"
              caret="1" popupmenu="1"/>
  <LABEL name="" id="ab64af59d8be5fe3" memberName="m_songLabel" virtualName=""
         explicitFocusOrder="0" pos="0 1 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Song" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <TEXTEDITOR name="" id="21ef937502d210df" memberName="m_songName" virtualName=""
              explicitFocusOrder="0" pos="136 2 272 22" initialText="" multiline="0"
              retKeyStartsLine="0" readonly="0" scrollbars="0" caret="1" popupmenu="1"/>
  <TOGGLEBUTTON name="" id="e7ecdde1129e3bb5" memberName="m_mono" virtualName=""
                explicitFocusOrder="0" pos="424 0 80 24" buttonText="Mono" connectedEdges="0"
                needsCallback="1" radioGroupId="0" state="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

