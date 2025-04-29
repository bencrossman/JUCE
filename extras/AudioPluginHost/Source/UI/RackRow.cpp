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

#include "RackRow.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
#include "GraphEditorPanel.h"
#include "../Plugins/PluginGraph.h"
#include "../Plugins/InternalPlugins.h"
#include "../examples/Plugins/GainPluginDemo.h"
String FormatKey(int note);
int ParseNote(const char *str);
float RackRow::m_tempo = 120;
//[/MiscUserDefs]

//==============================================================================
RackRow::RackRow ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    m_keyboardState = new MidiKeyboardState();
    m_soloMode = false;
    m_current = NULL;
    m_lastNote = -1;
    m_arpeggiatorBeat = 0;
    m_notesDown.reserve(128);
    m_arpeggiatorTimer = 0.f;
    //[/Constructor_pre]

    m_deviceName.reset (new juce::GroupComponent (juce::String(),
                                                  juce::String()));
    addAndMakeVisible (m_deviceName.get());

    m_deviceName->setBounds (0, -2, 816, 80);

    m_solo.reset (new juce::ToggleButton (juce::String()));
    addAndMakeVisible (m_solo.get());
    m_solo->setButtonText (TRANS("Solo"));
    m_solo->addListener (this);

    m_solo->setBounds (96, 14, 72, 24);

    m_mute.reset (new juce::ToggleButton (juce::String()));
    addAndMakeVisible (m_mute.get());
    m_mute->setButtonText (TRANS("Mute"));
    m_mute->addListener (this);

    m_mute->setBounds (160, 14, 72, 24);

    m_volume.reset (new juce::Slider (juce::String()));
    addAndMakeVisible (m_volume.get());
    m_volume->setRange (-110, 12, 0.5);
    m_volume->setSliderStyle (juce::Slider::LinearBar);
    m_volume->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    m_volume->addListener (this);

    m_volume->setBounds (96, 43, 128, 24);

    m_bank.reset (new juce::ComboBox (juce::String()));
    addAndMakeVisible (m_bank.get());
    m_bank->setEditableText (false);
    m_bank->setJustificationType (juce::Justification::centredLeft);
    m_bank->setTextWhenNothingSelected (juce::String());
    m_bank->setTextWhenNoChoicesAvailable (juce::String());
    m_bank->addSeparator();
    m_bank->addSeparator();
    m_bank->addListener (this);

    m_bank->setBounds (233, 43, 150, 24);

    m_program.reset (new juce::ComboBox (juce::String()));
    addAndMakeVisible (m_program.get());
    m_program->setEditableText (false);
    m_program->setJustificationType (juce::Justification::centredLeft);
    m_program->setTextWhenNothingSelected (juce::String());
    m_program->setTextWhenNoChoicesAvailable (juce::String());
    m_program->addListener (this);

    m_program->setBounds (233, 14, 150, 24);

    m_transpose.reset (new juce::TextEditor (juce::String()));
    addAndMakeVisible (m_transpose.get());
    m_transpose->setMultiLine (false);
    m_transpose->setReturnKeyStartsNewLine (false);
    m_transpose->setReadOnly (false);
    m_transpose->setScrollbarsShown (false);
    m_transpose->setCaretVisible (true);
    m_transpose->setPopupMenuEnabled (true);
    m_transpose->setText (juce::String());

    m_transpose->setBounds (648, 14, 32, 24);

    m_to.reset (new juce::Label (juce::String(),
                                 TRANS("to")));
    addAndMakeVisible (m_to.get());
    m_to->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    m_to->setJustificationType (juce::Justification::centred);
    m_to->setEditable (false, false, false);
    m_to->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    m_to->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    m_to->setBounds (744, 14, 24, 24);

    m_lowKey.reset (new juce::TextEditor (juce::String()));
    addAndMakeVisible (m_lowKey.get());
    m_lowKey->setMultiLine (false);
    m_lowKey->setReturnKeyStartsNewLine (false);
    m_lowKey->setReadOnly (false);
    m_lowKey->setScrollbarsShown (false);
    m_lowKey->setCaretVisible (true);
    m_lowKey->setPopupMenuEnabled (true);
    m_lowKey->setText (juce::String());

    m_lowKey->setBounds (712, 14, 32, 24);

    m_highKey.reset (new juce::TextEditor (juce::String()));
    addAndMakeVisible (m_highKey.get());
    m_highKey->setMultiLine (false);
    m_highKey->setReturnKeyStartsNewLine (false);
    m_highKey->setReadOnly (false);
    m_highKey->setScrollbarsShown (false);
    m_highKey->setCaretVisible (true);
    m_highKey->setPopupMenuEnabled (true);
    m_highKey->setText (juce::String());

    m_highKey->setBounds (768, 14, 32, 24);

    m_deviceSettings.reset (new juce::ImageButton (juce::String()));
    addAndMakeVisible (m_deviceSettings.get());
    m_deviceSettings->setButtonText (TRANS("new button"));
    m_deviceSettings->addListener (this);

    m_deviceSettings->setImages (false, true, true,
                                 juce::Image(), 1.000f, juce::Colour (0x00000000),
                                 juce::Image(), 1.000f, juce::Colour (0x00000000),
                                 juce::Image(), 1.000f, juce::Colour (0x00000000));
    m_deviceSettings->setBounds (8, 14, 76, 57);

    m_keyboard.reset (new MidiKeyboardComponent (*m_keyboardState, MidiKeyboardComponent::Orientation::horizontalKeyboard));
    addAndMakeVisible (m_keyboard.get());

    m_keyboard->setBounds (392, 43, 416, 24);

    m_noteMode.reset (new juce::ComboBox (juce::String()));
    addAndMakeVisible (m_noteMode.get());
    m_noteMode->setEditableText (false);
    m_noteMode->setJustificationType (juce::Justification::centredLeft);
    m_noteMode->setTextWhenNothingSelected (juce::String());
    m_noteMode->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    m_noteMode->addItem (TRANS("Normal note mode"), 1);
    m_noteMode->addItem (TRANS("Scoop"), 2);
    m_noteMode->addItem (TRANS("Fall"), 3);
    m_noteMode->addItem (TRANS("Sixteenth"), 4);
    m_noteMode->addItem (TRANS("Double octave"), 5);
    m_noteMode->addItem (TRANS("Three octave arpeggio"), 6);
    m_noteMode->addListener (this);

    m_noteMode->setBounds (400, 14, 232, 24);


    //[UserPreSize]
    m_keyboard->setKeyWidth(8.f);
    //[/UserPreSize]

    setSize (816, 76);


    //[Constructor] You can add your own custom stuff here..
    m_keyboard->addMouseListener(this, false);
    m_keyboard->setAvailableRange(21, 21+88-1);
    UpdateKeyboard();
    m_transpose->addListener(this);
    m_lowKey->addListener(this);
    m_highKey->addListener(this);
    m_deviceSettings->setTriggeredOnMouseDown(true);
    //[/Constructor]
}

RackRow::~RackRow()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    if (m_midiFilterNode)
        InternalPluginFormat::SetFilterCallback(m_midiFilterNode, NULL); // if we're still getting crashing related to this then probably because the node is already dead
    //[/Destructor_pre]

    m_deviceName = nullptr;
    m_solo = nullptr;
    m_mute = nullptr;
    m_volume = nullptr;
    m_bank = nullptr;
    m_program = nullptr;
    m_transpose = nullptr;
    m_to = nullptr;
    m_lowKey = nullptr;
    m_highKey = nullptr;
    m_deviceSettings = nullptr;
    m_keyboard = nullptr;
    m_noteMode = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
	delete m_keyboardState;
    //[/Destructor]
}

//==============================================================================
void RackRow::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    g.setColour(Colours::black);
    auto rect = m_deviceSettings->getBounds();
    rect.expand(1, 1);
    g.drawRect(rect);
    if (m_mute->getToggleState())
    {
        g.fillAll(Colour(0x50ffffff));
    }
    //[/UserPaint]
}

void RackRow::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void RackRow::buttonClicked (juce::Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == m_solo.get())
    {
        //[UserButtonCode_m_solo] -- add your button handler code here..
        m_current->Solo = buttonThatWasClicked->getToggleState();
        panel->SoloChange();
        //[/UserButtonCode_m_solo]
    }
    else if (buttonThatWasClicked == m_mute.get())
    {
        //[UserButtonCode_m_mute] -- add your button handler code here..
        m_current->Mute = buttonThatWasClicked->getToggleState();
        repaint(); // to change background of row
        if (m_current->Device->m_node)
        {
            auto bypass = m_current->Mute || (m_soloMode && !m_current->Solo);
            if (bypass)
            {
                m_pendingSoundOff = true;
                m_pendingBypass = true;
            }
			else
			{
				((AudioProcessorGraph::Node*)m_current->Device->m_node)->setBypassed(bypass);
				((AudioProcessorGraph::Node*)m_current->Device->m_midiFilterNode)->setBypassed(bypass);
				((AudioProcessorGraph::Node*)m_current->Device->m_gainNode)->setBypassed(bypass);
			}
        }
        m_program->setEnabled(!m_current->Mute);
        m_bank->setEnabled(!m_current->Mute);
        //[/UserButtonCode_m_mute]
    }
    else if (buttonThatWasClicked == m_deviceSettings.get())
    {
        //[UserButtonCode_m_deviceSettings] -- add your button handler code here..
        auto modifiers = ModifierKeys::getCurrentModifiers();
        if (modifiers.isRightButtonDown())
        {
            PopupMenu menu;
            menu.addItem(1, "Save global rack state");
            menu.addItem(2, "Save performance override state");
            menu.addItem(3, "Delete Rack");
            auto res = menu.show();
            if (res == 1 || res == 2)
            {
                MemoryBlock mb;
                ((AudioProcessorGraph::Node*)m_current->Device->m_node)->getProcessor()->getStateInformation(mb);
                MemoryInputStream uncompressedInput(mb.getData(), mb.getSize(), false);
                MemoryOutputStream output;
                GZIPCompressorOutputStream zipper(&output);
                zipper.writeFromInputStream(uncompressedInput, -1);
                zipper.flush();
                MemoryOutputStream output2;
                Base64::convertToBase64(output2, output.getData(),output.getDataSize());
                if (res == 1)
                    m_current->Device->InitialState = (const char*)output2.getData();
                else
                    m_current->OverrideState = (const char*)output2.getData();
            }
            if (res == 3)
            {

            }
        }
        else
        {
            if (m_current->Device->m_node)
                if (auto w = graph->getOrCreateWindowFor((AudioProcessorGraph::Node*)m_current->Device->m_node, PluginWindow::Type::normal))
                    w->toFront(true);
        }


        //[/UserButtonCode_m_deviceSettings]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void RackRow::sliderValueChanged (juce::Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == m_volume.get())
    {
        //[UserSliderCode_m_volume] -- add your slider handling code here..
        m_current->Volume = (float)sliderThatWasMoved->getValue();
		((AudioProcessorGraph::Node *)(m_current->Device->m_gainNode))->getProcessor()->getParameters()[0]->setValue(Decibels::decibelsToGain(m_current->Volume));
        //[/UserSliderCode_m_volume]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void RackRow::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == m_bank.get())
    {
        //[UserComboBoxCode_m_bank] -- add your combo box handling code here..
        m_current->Bank = m_bank->getSelectedId() - 1;
        m_current->Program = 0;

        m_pendingBank = true;
        m_pendingProgram = true;
        m_pendingProgramNames = true;
        //[/UserComboBoxCode_m_bank]
    }
    else if (comboBoxThatHasChanged == m_program.get())
    {
        //[UserComboBoxCode_m_program] -- add your combo box handling code here..
        if (m_program->getSelectedId() > 0)
        {
            m_current->Program = m_program->getSelectedId() - 1;
            m_pendingProgram = true;
        }
        //[/UserComboBoxCode_m_program]
    }
    else if (comboBoxThatHasChanged == m_noteMode.get())
    {
        //[UserComboBoxCode_m_noteMode] -- add your combo box handling code here..
        m_current->NoteMode = m_noteMode->getSelectedId() - 1;
        //[/UserComboBoxCode_m_noteMode]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void RackRow::mouseDown (const juce::MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    mouseDrag(e);
    //[/UserCode_mouseDown]
}

void RackRow::mouseDrag (const juce::MouseEvent& e)
{
    //[UserCode_mouseDrag] -- Add your code here...
    if (e.eventComponent == m_keyboard.get())
    {
        auto key = m_keyboard->getNoteAtPosition(e.position);
        if (key != -1)
        {
            auto lowkey = ParseNote(m_lowKey->getTextValue().toString().getCharPointer());
            auto highkey = ParseNote(m_highKey->getTextValue().toString().getCharPointer());
            if (abs(key - lowkey) > abs(key - highkey))
            {
                m_highKey->setText(FormatKey(key));
                m_current->HighKey = key;
            }
            else
            {
                m_lowKey->setText(FormatKey(key));
                m_current->LowKey = key;
            }
            UpdateKeyboard();
        }
    }
    //[/UserCode_mouseDrag]
}

void RackRow::mouseUp (const juce::MouseEvent& e)
{
    //[UserCode_mouseUp] -- Add your code here...
    mouseDrag(e);
    //[/UserCode_mouseUp]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void RackRow::textEditorTextChanged(TextEditor&te)
{
    if (&te == m_transpose.get())
    {
        m_current->Transpose = atoi(te.getText().getCharPointer());
    }
    else if (&te == m_lowKey.get())
    {
        m_current->LowKey = ParseNote(te.getText().getCharPointer());
        UpdateKeyboard();
    }
    else if (&te == m_highKey.get())
    {
        m_current->HighKey = ParseNote(te.getText().getCharPointer());
        UpdateKeyboard();
    }
}

String FormatKey(int note)
{
    const char *notenames[] = { "C ","C#","D ","D#","E ","F ","F#","G ","G#","A ","A#","B " };
    return String::formatted("%S%d", notenames[note % 12], note / 12 - 2);
}

int ParseNote(const char *str)
{
    char notename[256];

    char nospace[256];
    memset(nospace, 0, sizeof(nospace));
    for (int c = 0; c<(int)strlen(str); ++c)
    {
        if (str[c] != ' ')
            nospace[strlen(nospace)] = str[c];
    }
    const char *notenames[] = { "C","C#","D","D#","E","F","F#","G","G#","A","A#","B" };
    for (int note = 0; note <= 127; ++note)
    {
        sprintf(notename, "%s%d", notenames[note % 12], note / 12 - 2);
        if (_stricmp(notename, nospace) == 0)
            return note;
    }
    return 0;
}

void RackRow::UpdateKeyboard()
{
    auto lowkey = ParseNote(m_lowKey->getTextValue().toString().getCharPointer());
    auto highkey = ParseNote(m_highKey->getTextValue().toString().getCharPointer());
    m_keyboardState->allNotesOff(1);
    for (int i = 0; i < lowkey; ++i)
        m_keyboardState->noteOn(1, i, 1.0f);
    for (int i = highkey + 1; i<128; ++i)
        m_keyboardState->noteOn(1, i, 1.0f);
}

void RackRow::Filter(int samples, int sampleRate, MidiBuffer &midiBuffer)
{
    int arpeggiatorSample = 0;

    if (!midiBuffer.isEmpty())
    {
        MidiBuffer output;

		for(const auto meta : midiBuffer)
        {
			auto midi_message = meta.getMessage();
			int sample_number = meta.samplePosition;

			if (m_current == NULL)
                continue; // whilst I test the song knobs
            midi_message.setChannel(1);
            if (midi_message.isNoteOnOrOff())
            {
                if (midi_message.getNoteNumber() >= m_current->LowKey && midi_message.getNoteNumber() <= m_current->HighKey)
                {
                    int note = midi_message.getNoteNumber() + m_current->Transpose;
                    if (note >= 0 && note <= 127)
                    {
                        if (m_current->NoteMode == NoteMode::Sixteenth || m_current->NoteMode == NoteMode::ThreeOctaveArpeggio)
                        {
                            // See if new sequence
                            if (m_notesDown.empty() && midi_message.isNoteOn())
                            {
                                m_arpeggiatorBeat = 0;
                                m_arpeggiatorTimer = 0.001f; // 1ms for first one
                                arpeggiatorSample = sample_number;
                            }

                            // Update m_notesDown with new info
                            int found = -1;
                            for (auto i = 0U; i < m_notesDown.size(); ++i)
                                if (m_notesDown[i] == note)
                                    found = i;
                            if (found == -1 && midi_message.isNoteOn())
                            {
                                for (auto i = 0U; i < m_notesDown.size(); ++i)
                                {
                                    if (m_notesDown[i] > note)
                                    {
                                        found = i;
                                        break;
                                    }
                                }
                                if (found == -1)
                                    m_notesDown.push_back(note);
                                else
                                    m_notesDown.insert(m_notesDown.begin() + found, note);
                                m_arpeggiatorBeat = 0; // Any change in notes resets arpeggiator position
                            }
                            if (found != -1 && midi_message.isNoteOff())
                            {
                                swap(m_notesDown[found], m_notesDown[m_notesDown.size() - 1]);
                                m_notesDown.pop_back();
                                m_arpeggiatorBeat = 0; // Any change in notes resets arpeggiator position
                            }

                            // are we ending?
                            if (midi_message.isNoteOff() && m_notesDown.empty())
                            {
                                // cancel last note
                                if (m_lastNote >= 0)
                                {
                                    output.addEvent(MidiMessage::noteOff(1, m_lastNote), sample_number);
                                    m_lastNote = -1;
                                }
                                m_arpeggiatorTimer = 0.f;
                            }
                        }
                        else
                        {
                            midi_message.setNoteNumber(note); // transposed note
                            output.addEvent(midi_message, sample_number);
                            if (m_current->NoteMode == NoteMode::DoubleOctave && note < 128 - 12) // double octave
                            {
                                midi_message.setNoteNumber(note + 12);
                                output.addEvent(midi_message, sample_number);
                            }
                        }
                    }
                }
            }
            else
                output.addEvent(midi_message, sample_number); // other events like sustain
        }
        midiBuffer = output;
    }

    if (m_pendingSoundOff)
    {
		// turn off all notes, OPX didnt work with just MidiMessage::AllNotesOff
		for (int note = 0; note <= 127; ++note)
			midiBuffer.addEvent(MidiMessage::noteOff(1, note),0);

        midiBuffer.addEvent(MidiMessage::allSoundOff(1), 0);
        m_pendingSoundOff = false;
    }
    else if (m_pendingBank)
    {
        m_pendingBank = false;
        if (m_current->Device->m_usesBanks)
        {
            midiBuffer.addEvent(MidiMessage(0xB0, 0x00, 0), 0);
            midiBuffer.addEvent(MidiMessage(0xB0, 0x20, m_current->Bank), 0);
        }
    }
    else if (m_pendingProgramNames) // need bank to change first
    {
        m_pendingProgramNames = false;
        if (!m_manualPatchNames)
            postCommandMessage(CommandUpdateProgramList);
    }
    else if (m_pendingProgram) // need bank to change first
    {
        m_pendingProgram = false;
        midiBuffer.addEvent(MidiMessage(0xC0, m_current->Program), 0); // I think this is needed to trigger the bank change too
    }
    else if (m_pendingBypass) // do this last so nothing is buffered up. Streamer should be fine since should be disabled(0)
    {
        m_pendingBypass = false;
        postCommandMessage(CommandBypass);
    }

    if (m_arpeggiatorTimer > 0) // arpeggiator active
    {
        while (1)
        {
            auto nextarpeggiatorSample = int(arpeggiatorSample + m_arpeggiatorTimer * sampleRate);
            if (nextarpeggiatorSample < samples) // beat in this block
            {
                arpeggiatorSample = nextarpeggiatorSample; // in case we can fit more in

                m_arpeggiatorTimer = 15.f / m_tempo; // reset timer

                // cancel last note
                if (m_lastNote >= 0)
                {
                    midiBuffer.addEvent(MidiMessage::noteOff(1, m_lastNote), nextarpeggiatorSample);
                    m_lastNote = -1;
                }

                // need new note
                if (!m_notesDown.empty())
                {
                    m_lastNote = m_notesDown[m_arpeggiatorBeat % m_notesDown.size()] + 12 * ((m_arpeggiatorBeat / m_notesDown.size()) % (m_current->NoteMode == NoteMode::ThreeOctaveArpeggio ? 3 : 1));
                    if (m_lastNote < 128)
                        midiBuffer.addEvent(MidiMessage::noteOn(1, m_lastNote, 1.0f), nextarpeggiatorSample);
                    m_arpeggiatorBeat++;
                }
            }
            else
                break;
        }

        // reduce timer
        m_arpeggiatorTimer -= (samples - arpeggiatorSample) / (float)sampleRate;
        assert(m_arpeggiatorTimer > 0); // shouldnt be possible to get to 0 here
    }
}

void RackRow::Setup(Device &device, PluginGraph &pluginGraph, GraphEditorPanel &GraphEditorPanel)
{
    graph = &pluginGraph;
    panel = &GraphEditorPanel;

    m_deviceName->setText(device.Name);
    auto image = ImageFileFormat::loadFrom(File::getCurrentWorkingDirectory().getFullPathName() + "\\" + String(device.Name + ".png"));
    m_deviceSettings->setImages(false, false, false, image, 1.0f, Colours::transparentBlack, image, 1.0f, Colours::transparentBlack, image, 1.0f, Colours::transparentBlack);

	m_midiFilterNode = (AudioProcessorGraph::Node*)device.m_midiFilterNode;
    if (m_midiFilterNode)
        InternalPluginFormat::SetFilterCallback(m_midiFilterNode, this);

    m_id = device.ID;

    auto bankFile = File::getCurrentWorkingDirectory().getFullPathName() + "\\" + String(device.PluginName + "_Banks.txt");
    if (File(bankFile).exists())
    {
        StringArray lines;
        File(bankFile).readLines(lines);
        for (int i = 0; i < lines.size(); ++i)
            m_bank->addItem(lines[i], i + 1);
    }
    else
        m_bank->setVisible(false);

    auto programFile = File::getCurrentWorkingDirectory().getFullPathName() + "\\" + String(device.Name + ".txt");
    if (File(programFile).exists())
    {
        m_manualPatchNames = true;
        StringArray lines;
        File(programFile).readLines(lines);
        for (int i = 0; i<lines.size(); ++i)
            m_program->addItem(lines[i], i + 1);
    }
    else if (!m_bank->isVisible() && device.m_node)
    {
        auto processor = ((AudioProcessorGraph::Node*)device.m_node)->getProcessor();

        for (int i = 0; i < processor->getNumPrograms(); ++i)
            m_program->addItem(processor->getProgramName(i).isNotEmpty() ? processor->getProgramName(i) : String(i+1), i + 1);
    }
}

void RackRow::Assign(Zone *zone)
{
    m_current = zone;

    auto processor = (AudioPluginInstance *)((AudioProcessorGraph::Node*)zone->Device->m_node)->getProcessor();
    if (!zone->OverrideState.empty())
    {
        graph->SendChunkString(processor, zone->OverrideState);
        m_lastZoneHadOverrideState = true;
    }
    else
    {
        if (m_lastZoneHadOverrideState)
        {
            graph->SendChunkString(processor, zone->Device->InitialState);
            m_lastZoneHadOverrideState = false;
        }

        m_pendingBank = true;
        m_pendingProgram = true;
        m_pendingSoundOff = true;
        m_pendingProgramNames = true;
    }
    m_volume->setValue(zone->Volume);
    m_solo->setToggleState(zone->Solo, sendNotification); // some logic in these two so better do it
    m_mute->setToggleState(zone->Mute, sendNotification);
	m_noteMode->setSelectedItemIndex(zone->NoteMode, dontSendNotification);
    m_lowKey->setText(FormatKey(zone->LowKey));
    m_highKey->setText(FormatKey(zone->HighKey));
    m_transpose->setText(String(zone->Transpose));

    if (m_bank->isVisible())
        m_bank->setSelectedId(zone->Bank + 1, dontSendNotification);

    m_program->setSelectedId(zone->Program + 1, dontSendNotification);


    UpdateKeyboard();
}

void RackRow::SetSoloMode(bool mode)
{
	m_soloMode = mode;
	// Do this here again. Can't rely on Toggle because only works if changed
	((AudioProcessorGraph::Node*)m_current->Device->m_node)->setBypassed(m_current->Mute || (m_soloMode && !m_current->Solo));
	((AudioProcessorGraph::Node*)m_current->Device->m_gainNode)->setBypassed(m_current->Mute || (m_soloMode && !m_current->Solo));
	((AudioProcessorGraph::Node*)m_current->Device->m_midiFilterNode)->setBypassed(m_current->Mute || (m_soloMode && !m_current->Solo));
}

void RackRow::handleCommandMessage(int id)
{
    if (id == CommandUpdateProgramList)
    {
        auto processor = ((AudioProcessorGraph::Node*)m_current->Device->m_node)->getProcessor();
        m_program->clear(dontSendNotification);

		if (processor->getNumPrograms() == 1 && processor->getProgramName(0) == "")
		{
			for (int i = 0; i < 128; ++i)
				m_program->addItem(String::formatted("PROGRAM %03d", i), i + 1);
		}
		else
		{
			for (int i = 0; i < processor->getNumPrograms(); ++i)
				m_program->addItem(processor->getProgramName(i), i + 1);
		}
        m_program->setSelectedId(m_current->Program + 1, dontSendNotification);
    }
    if (id == CommandBypass)
    {
		((AudioProcessorGraph::Node*)m_current->Device->m_node)->setBypassed(true);
		((AudioProcessorGraph::Node*)m_current->Device->m_midiFilterNode)->setBypassed(true);
		((AudioProcessorGraph::Node*)m_current->Device->m_gainNode)->setBypassed(true);
	}
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="RackRow" componentName=""
                 parentClasses="public Component, public TextEditor::Listener, public MidiFilterCallback"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="816"
                 initialHeight="76">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
    <METHOD name="mouseDrag (const MouseEvent&amp; e)"/>
    <METHOD name="mouseUp (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <GROUPCOMPONENT name="" id="85efcbef1342dec0" memberName="m_deviceName" virtualName=""
                  explicitFocusOrder="0" pos="0 -2 816 80" title=""/>
  <TOGGLEBUTTON name="" id="2b62ef4a67b701f3" memberName="m_solo" virtualName=""
                explicitFocusOrder="0" pos="96 14 72 24" buttonText="Solo" connectedEdges="0"
                needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="" id="6ad6011b41b23475" memberName="m_mute" virtualName=""
                explicitFocusOrder="0" pos="160 14 72 24" buttonText="Mute" connectedEdges="0"
                needsCallback="1" radioGroupId="0" state="0"/>
  <SLIDER name="" id="a0e2bc5a61933c6d" memberName="m_volume" virtualName=""
          explicitFocusOrder="0" pos="96 43 128 24" min="-110.0" max="12.0"
          int="0.5" style="LinearBar" textBoxPos="TextBoxBelow" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <COMBOBOX name="" id="90d63ca95a92a112" memberName="m_bank" virtualName=""
            explicitFocusOrder="0" pos="233 43 150 24" editable="0" layout="33"
            items="&#10;" textWhenNonSelected="" textWhenNoItems=""/>
  <COMBOBOX name="" id="9de3cb5469378fa1" memberName="m_program" virtualName=""
            explicitFocusOrder="0" pos="233 14 150 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems=""/>
  <TEXTEDITOR name="" id="b6e30577b79a003a" memberName="m_transpose" virtualName=""
              explicitFocusOrder="0" pos="648 14 32 24" initialText="" multiline="0"
              retKeyStartsLine="0" readonly="0" scrollbars="0" caret="1" popupmenu="1"/>
  <LABEL name="" id="72d9777463cc6a85" memberName="m_to" virtualName=""
         explicitFocusOrder="0" pos="744 14 24 24" edTextCol="ff000000"
         edBkgCol="0" labelText="to" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <TEXTEDITOR name="" id="3d470180923a3d6f" memberName="m_lowKey" virtualName=""
              explicitFocusOrder="0" pos="712 14 32 24" initialText="" multiline="0"
              retKeyStartsLine="0" readonly="0" scrollbars="0" caret="1" popupmenu="1"/>
  <TEXTEDITOR name="" id="5f3abd7bbb50678c" memberName="m_highKey" virtualName=""
              explicitFocusOrder="0" pos="768 14 32 24" initialText="" multiline="0"
              retKeyStartsLine="0" readonly="0" scrollbars="0" caret="1" popupmenu="1"/>
  <IMAGEBUTTON name="" id="31b2ae44720b5f47" memberName="m_deviceSettings" virtualName=""
               explicitFocusOrder="0" pos="8 14 76 57" buttonText="new button"
               connectedEdges="0" needsCallback="1" radioGroupId="0" keepProportions="1"
               resourceNormal="" opacityNormal="1.0" colourNormal="0" resourceOver=""
               opacityOver="1.0" colourOver="0" resourceDown="" opacityDown="1.0"
               colourDown="0"/>
  <GENERICCOMPONENT name="" id="3a433662794e0409" memberName="m_keyboard" virtualName="MidiKeyboardComponent"
                    explicitFocusOrder="0" pos="392 43 416 24" class="unknown" params="*m_keyboardState, MidiKeyboardComponent::Orientation::horizontalKeyboard"/>
  <COMBOBOX name="" id="321cb138fb836f9e" memberName="m_noteMode" virtualName=""
            explicitFocusOrder="0" pos="400 14 232 24" editable="0" layout="33"
            items="Normal note mode&#10;Scoop&#10;Fall&#10;Sixteenth&#10;Double octave&#10;Three octave arpeggio"
            textWhenNonSelected="" textWhenNoItems="(no choices)"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

