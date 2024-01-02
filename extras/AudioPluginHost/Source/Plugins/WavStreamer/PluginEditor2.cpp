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
#include "PluginProcessor.h"
//[/Headers]

#include "PluginEditor.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
#define _CRT_SECURE_NO_WARNINGS
//[/MiscUserDefs]

//==============================================================================
FilePlaybackPluginAudioProcessorEditor::FilePlaybackPluginAudioProcessorEditor (AudioProcessor& processor)
    : AudioProcessorEditor(processor)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    m_addButton.reset (new TextButton (String()));
    addAndMakeVisible (m_addButton.get());
    m_addButton->setButtonText (TRANS("Add"));
    m_addButton->addListener (this);

    m_addButton->setBounds (8, 8, 80, 24);

    m_deleteButton.reset (new TextButton (String()));
    addAndMakeVisible (m_deleteButton.get());
    m_deleteButton->setButtonText (TRANS("Delete"));
    m_deleteButton->addListener (this);

    m_deleteButton->setBounds (96, 8, 80, 24);

    m_moveUp.reset (new TextButton (String()));
    addAndMakeVisible (m_moveUp.get());
    m_moveUp->setButtonText (TRANS("Move Up"));
    m_moveUp->addListener (this);

    m_moveUp->setBounds (184, 8, 80, 24);

    m_moveDown.reset (new TextButton (String()));
    addAndMakeVisible (m_moveDown.get());
    m_moveDown->setButtonText (TRANS("Move Down"));
    m_moveDown->addListener (this);

    m_moveDown->setBounds (272, 8, 80, 24);

    m_table.reset (new TableDemoComponent());
    addAndMakeVisible (m_table.get());

    m_table->setBounds (8, 40, 432, 352);

    m_toggleLoop.reset (new TextButton (String()));
    addAndMakeVisible (m_toggleLoop.get());
    m_toggleLoop->setButtonText (TRANS("Toggle Loop"));
    m_toggleLoop->addListener (this);

    m_toggleLoop->setBounds (360, 8, 80, 24);


    //[UserPreSize]
    m_table->SetList(((FilePlaybackPluginAudioProcessor*)&processor)->GetList());

    //[/UserPreSize]

    setSize (448, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

FilePlaybackPluginAudioProcessorEditor::~FilePlaybackPluginAudioProcessorEditor()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    m_addButton = nullptr;
    m_deleteButton = nullptr;
    m_moveUp = nullptr;
    m_moveDown = nullptr;
    m_table = nullptr;
    m_toggleLoop = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void FilePlaybackPluginAudioProcessorEditor::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::white);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void FilePlaybackPluginAudioProcessorEditor::resized()
{
    //[UserPreResize] Add your own custom resize code here..

    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void FilePlaybackPluginAudioProcessorEditor::buttonClicked (Button* buttonThatWasClicked)
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
    else if (buttonThatWasClicked == m_moveUp.get())
    {
        //[UserButtonCode_m_moveUp] -- add your button handler code here..
        m_table->MoveUpSelected();
        //[/UserButtonCode_m_moveUp]
    }
    else if (buttonThatWasClicked == m_moveDown.get())
    {
        //[UserButtonCode_m_moveDown] -- add your button handler code here..
        m_table->MoveDownSelected();
        //[/UserButtonCode_m_moveDown]
    }
    else if (buttonThatWasClicked == m_toggleLoop.get())
    {
        //[UserButtonCode_m_toggleLoop] -- add your button handler code here..
		m_table->ToggleSelectedLoop();
        //[/UserButtonCode_m_toggleLoop]
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

<JUCER_COMPONENT documentType="Component" className="FilePlaybackPluginAudioProcessorEditor"
                 componentName="" parentClasses="public AudioProcessorEditor"
                 constructorParams="AudioProcessor&amp; processor" variableInitialisers="AudioProcessorEditor(processor)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="448" initialHeight="400">
  <BACKGROUND backgroundColour="ffffffff"/>
  <TEXTBUTTON name="" id="62dcb6cad5fd83b" memberName="m_addButton" virtualName=""
              explicitFocusOrder="0" pos="8 8 80 24" buttonText="Add" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="ee0ae4bd0107d78c" memberName="m_deleteButton" virtualName=""
              explicitFocusOrder="0" pos="96 8 80 24" buttonText="Delete" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="1b0fe6caa5b71f46" memberName="m_moveUp" virtualName=""
              explicitFocusOrder="0" pos="184 8 80 24" buttonText="Move Up"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="b6031428c6fd9778" memberName="m_moveDown" virtualName=""
              explicitFocusOrder="0" pos="272 8 80 24" buttonText="Move Down"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <GENERICCOMPONENT name="" id="2e2e5afc809d8716" memberName="m_table" virtualName=""
                    explicitFocusOrder="0" pos="8 40 432 352" class="TableDemoComponent"
                    params=""/>
  <TEXTBUTTON name="" id="74227fc19ec4b7cc" memberName="m_toggleLoop" virtualName=""
              explicitFocusOrder="0" pos="360 8 80 24" buttonText="Toggle Loop"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

