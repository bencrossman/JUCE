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

    m_transposeColumn->setBounds (628, 0, 72, 24);

    m_bankProgramColumn.reset (new Label (String(),
                                          TRANS("Bank/Program")));
    addAndMakeVisible (m_bankProgramColumn.get());
    m_bankProgramColumn->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    m_bankProgramColumn->setJustificationType (Justification::centred);
    m_bankProgramColumn->setEditable (false, false, false);
    m_bankProgramColumn->setColour (TextEditor::textColourId, Colours::black);
    m_bankProgramColumn->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    m_bankProgramColumn->setBounds (233, 0, 150, 24);

    m_rangeColumn.reset (new Label (String(),
                                    TRANS("Range")));
    addAndMakeVisible (m_rangeColumn.get());
    m_rangeColumn->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    m_rangeColumn->setJustificationType (Justification::centred);
    m_rangeColumn->setEditable (false, false, false);
    m_rangeColumn->setColour (TextEditor::textColourId, Colours::black);
    m_rangeColumn->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    m_rangeColumn->setBounds (724, 0, 64, 24);

    m_volumeColumn.reset (new Label (String(),
                                     TRANS("Volume")));
    addAndMakeVisible (m_volumeColumn.get());
    m_volumeColumn->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    m_volumeColumn->setJustificationType (Justification::centred);
    m_volumeColumn->setEditable (false, false, false);
    m_volumeColumn->setColour (TextEditor::textColourId, Colours::black);
    m_volumeColumn->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    m_volumeColumn->setBounds (96, 0, 128, 24);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (816, 24);


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



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="RackTitleBar" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="816" initialHeight="24">
  <BACKGROUND backgroundColour="ff323e44"/>
  <LABEL name="" id="df98b892f707f559" memberName="m_transposeColumn"
         virtualName="" explicitFocusOrder="0" pos="628 0 72 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Transpose" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="1ec7c2bf1356976c" memberName="m_bankProgramColumn"
         virtualName="" explicitFocusOrder="0" pos="233 0 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Bank/Program" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="45ccc111fb3f525b" memberName="m_rangeColumn" virtualName=""
         explicitFocusOrder="0" pos="724 0 64 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Range" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="7f6211731f4b9927" memberName="m_volumeColumn" virtualName=""
         explicitFocusOrder="0" pos="96 0 128 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Volume" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

