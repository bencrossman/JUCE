/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 7.0.2

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
#include "SoundFontPlayerProcessor.h"
//[/Headers]

#include "SoundFontPlayerEditor.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
SoundFontPlayerEditor::SoundFontPlayerEditor (AudioProcessor& processor)
    : AudioProcessorEditor(processor)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    m_setButton.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (m_setButton.get());
    m_setButton->setButtonText (TRANS("Set"));
    m_setButton->addListener (this);

    m_setButton->setBounds (8, 8, 80, 24);

    m_clearButton.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (m_clearButton.get());
    m_clearButton->setButtonText (TRANS("Clear"));
    m_clearButton->addListener (this);

    m_clearButton->setBounds (96, 8, 80, 24);

    m_table.reset (new SoundFontPlayerTableComponent());
    addAndMakeVisible (m_table.get());

    m_table->setBounds (8, 40, 432, 352);

    m_loadToggle.reset (new juce::ToggleButton (juce::String()));
    addAndMakeVisible (m_loadToggle.get());
    m_loadToggle->setButtonText (TRANS("Preload"));
    m_loadToggle->addListener (this);
    m_loadToggle->setToggleState (true, juce::dontSendNotification);
    m_loadToggle->setColour (juce::ToggleButton::textColourId, juce::Colours::white);

    m_loadToggle->setBounds (360, 8, 80, 24);


    //[UserPreSize]
    m_table->SetList(((SoundFontPlayerAudioProcessor*)&processor)->GetList());
    m_preload = ((SoundFontPlayerAudioProcessor*)&processor)->GetPreload();
    m_loadToggle->setToggleState(*m_preload, false);
    //[/UserPreSize]

    setSize (448, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

SoundFontPlayerEditor::~SoundFontPlayerEditor()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    m_setButton = nullptr;
    m_clearButton = nullptr;
    m_table = nullptr;
    m_loadToggle = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void SoundFontPlayerEditor::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff8c72ff));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void SoundFontPlayerEditor::resized()
{
    //[UserPreResize] Add your own custom resize code here..

    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void SoundFontPlayerEditor::buttonClicked (juce::Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == m_setButton.get())
    {
        //[UserButtonCode_m_setButton] -- add your button handler code here..

        FileChooser ChooseFile("Select wav file:",File(),"*.sf2");
        if(ChooseFile.browseForFileToOpen())
        {
          File fileName2=ChooseFile.getResult().withFileExtension("sf2");//make sure you end with desired extension

          m_table->SetSelected(fileName2.getFullPathName().toUTF8());
        }

        //[/UserButtonCode_m_setButton]
    }
    else if (buttonThatWasClicked == m_clearButton.get())
    {
        //[UserButtonCode_m_clearButton] -- add your button handler code here..
        m_table->DeleteSelected();
        //[/UserButtonCode_m_clearButton]
    }
    else if (buttonThatWasClicked == m_loadToggle.get())
    {
        //[UserButtonCode_m_loadToggle] -- add your button handler code here..
        *m_preload = m_loadToggle->getToggleState();
        //[/UserButtonCode_m_loadToggle]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SoundFontPlayerEditor" componentName=""
                 parentClasses="public AudioProcessorEditor" constructorParams="AudioProcessor&amp; processor"
                 variableInitialisers="AudioProcessorEditor(processor)" snapPixels="8"
                 snapActive="1" snapShown="1" overlayOpacity="0.330" fixedSize="1"
                 initialWidth="448" initialHeight="400">
  <BACKGROUND backgroundColour="ff8c72ff"/>
  <TEXTBUTTON name="" id="62dcb6cad5fd83b" memberName="m_setButton" virtualName=""
              explicitFocusOrder="0" pos="8 8 80 24" buttonText="Set" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="ee0ae4bd0107d78c" memberName="m_clearButton" virtualName=""
              explicitFocusOrder="0" pos="96 8 80 24" buttonText="Clear" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <GENERICCOMPONENT name="" id="2e2e5afc809d8716" memberName="m_table" virtualName=""
                    explicitFocusOrder="0" pos="8 40 432 352" class="SoundFontPlayerTableComponent"
                    params=""/>
  <TOGGLEBUTTON name="" id="92b2de9862e041fc" memberName="m_loadToggle" virtualName=""
                explicitFocusOrder="0" pos="360 8 80 24" txtcol="ffffffff" buttonText="Preload"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

