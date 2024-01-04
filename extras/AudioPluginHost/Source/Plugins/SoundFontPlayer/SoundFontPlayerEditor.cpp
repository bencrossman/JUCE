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
#define _CRT_SECURE_NO_WARNINGS
//[/MiscUserDefs]

//==============================================================================
SoundFontPlayerEditor::SoundFontPlayerEditor (AudioProcessor& processor)
    : AudioProcessorEditor(processor)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    m_addButton.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (m_addButton.get());
    m_addButton->setButtonText (TRANS("Set"));
    m_addButton->addListener (this);

    m_addButton->setBounds (8, 8, 80, 24);

    m_deleteButton.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (m_deleteButton.get());
    m_deleteButton->setButtonText (TRANS("Clear"));
    m_deleteButton->addListener (this);

    m_deleteButton->setBounds (96, 8, 80, 24);

    m_table.reset (new TableDemoComponent());
    addAndMakeVisible (m_table.get());

    m_table->setBounds (8, 40, 432, 352);


    //[UserPreSize]
    m_table->SetList(((SoundFontPlayerAudioProcessor*)&processor)->GetList());

    //[/UserPreSize]

    setSize (448, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

SoundFontPlayerEditor::~SoundFontPlayerEditor()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    m_addButton = nullptr;
    m_deleteButton = nullptr;
    m_table = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void SoundFontPlayerEditor::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colours::white);

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

    if (buttonThatWasClicked == m_addButton.get())
    {
        //[UserButtonCode_m_addButton] -- add your button handler code here..

        FileChooser ChooseFile("Select wav file:",File(),"*.wav");
        if(ChooseFile.browseForFileToOpen())
        {
          File fileName2=ChooseFile.getResult().withFileExtension("wav");//make sure you end with desired extension

          m_table->AddItem(fileName2.getFullPathName().toUTF8());
        }

        //[/UserButtonCode_m_addButton]
    }
    else if (buttonThatWasClicked == m_deleteButton.get())
    {
        //[UserButtonCode_m_deleteButton] -- add your button handler code here..
        m_table->DeleteSelected();
        //[/UserButtonCode_m_deleteButton]
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
  <BACKGROUND backgroundColour="ffffffff"/>
  <TEXTBUTTON name="" id="62dcb6cad5fd83b" memberName="m_addButton" virtualName=""
              explicitFocusOrder="0" pos="8 8 80 24" buttonText="Set" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="ee0ae4bd0107d78c" memberName="m_deleteButton" virtualName=""
              explicitFocusOrder="0" pos="96 8 80 24" buttonText="Clear" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <GENERICCOMPONENT name="" id="2e2e5afc809d8716" memberName="m_table" virtualName=""
                    explicitFocusOrder="0" pos="8 40 432 352" class="TableDemoComponent"
                    params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

