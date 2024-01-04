/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "GuitarStrummerProcessor.h"
#include "stdio.h"

//==============================================================================
GuitarStrummerAudioProcessor::GuitarStrummerAudioProcessor() : AudioProcessor (BusesProperties().withOutput ("Output", AudioChannelSet::stereo(), true))
{
}

GuitarStrummerAudioProcessor::~GuitarStrummerAudioProcessor()
{
}

//==============================================================================
const String GuitarStrummerAudioProcessor::getName() const
{
    return "Guitar Strummer";
}

bool GuitarStrummerAudioProcessor::acceptsMidi() const
{
    return true;
}

bool GuitarStrummerAudioProcessor::producesMidi() const
{
    return false;
}

bool GuitarStrummerAudioProcessor::isMidiEffect() const
{
    return false;
}

double GuitarStrummerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GuitarStrummerAudioProcessor::getNumPrograms()
{
    return 1;
}

int GuitarStrummerAudioProcessor::getCurrentProgram()
{
    return 0;
}



void GuitarStrummerAudioProcessor::setCurrentProgram (int )
{
}

const String GuitarStrummerAudioProcessor::getProgramName (int )
{
    return "Acoustic";
}

void GuitarStrummerAudioProcessor::changeProgramName (int, const String&)
{
}

//==============================================================================
void GuitarStrummerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    m_guitarChordPlayer.loadSoundfont(File(File::getCurrentWorkingDirectory().getFullPathName() + "\\GuitarStrummer.sf2"));
    m_guitarChordPlayer.prepareToPlay(samplesPerBlock, sampleRate);
    m_lastChordKey = -1;
    m_lastChordType = -1;
    dsp::ProcessSpec spec;
    spec.numChannels = 2;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    m_reverb.reset(new dsp::Reverb());
    m_reverb->prepare(spec);

    auto buffers = int(sampleRate * 0.08 / samplesPerBlock);
    if (buffers < 2)
        buffers = 2;
    m_buffer.resize(buffers);
}

void GuitarStrummerAudioProcessor::releaseResources()
{
    m_guitarChordPlayer.releaseResources();
}

bool GuitarStrummerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;
    return true;
}

void GuitarStrummerAudioProcessor::processBlockBypassed(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	if (m_reverbActive)
	{
		m_reverb->reset();
		m_reverbActive = false;
	}
}

void GuitarStrummerAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    
    int midiEventPos;
    MidiMessage m;
    MidiBuffer::Iterator midiIterator(midiMessages);

    // shuffle down
    for (int i = 1; i < m_buffer.size(); ++i)
        m_buffer[i - 1] = m_buffer[i];

    m_buffer.back().clear();

    while (auto found = midiIterator.getNextEvent(m, midiEventPos))
    {
        if (m.isNoteOn())
            m_buffer.back().push_back(m);
        if (m.isController())
            m_guitarChordPlayer.cc(m.getControllerNumber(), m.getControllerValue());
    }

    // get all events
    static std::vector<MidiMessage> chord;
    chord.clear();
    for (int i = 0; i < m_buffer.size(); ++i)
        for (int j = 0; j < m_buffer[i].size(); ++j)
            chord.push_back(m_buffer[i][j]);
            
    if (chord.size() >= 3)
    {

        // identify chord

        // remove octave notes
        bool scale[12];
        static std::vector<int> chords2, chords3;
        chords2.clear();
        chords3.clear();

        memset(scale, 0, sizeof(bool) * 12);
        for (int i = 0; i < chord.size(); ++i)
        {
            int note = chord[i].getNoteNumber() % 12;
            if (chord[i].isNoteOn() && !scale[note])
            {
                chords2.push_back(note);
                scale[note] = true;
            }
        }

        // try all notes as bottom
        int chordType = -1;
        int chordScore = 0;
        int key = 0;
        for (int i = 0; i < chords2.size(); ++i)
        {
            chords3 = chords2;
            for (int j = 0; j < chords2.size(); ++j)
                chords3[j] = (chords3[j] - chords2[i] + 120) % 12;
            std::sort(chords3.begin(), chords3.end());

            // may want to remove 1's, 6's, 8's, 9's

            int thisChordType = -1;
            if (chords3.size() == 3)
            {
                if (chords3[1] == 4 && chords3[2] == 7) // major
                    thisChordType = 1;
                else if (chords3[1] == 3 && chords3[2] == 7) // minor
                    thisChordType = 0;
                else if (chords3[1] == 5 && chords3[2] == 7) // sus4
                    thisChordType = 3;
            }
            if (chords3.size() == 4)
            {
                if (chords3[1] == 4 && chords3[2] == 7 && chords3[3] == 10) // m7
                    thisChordType = 2;
                else if (chords3[1] == 2 && chords3[2] == 4 && chords3[3] == 7) // +9
                    thisChordType = 4;
                else if (chords3[1] == 4 && chords3[2] == 7 && chords3[3] == 11) // M7
                    thisChordType = 5;
            }

            if (thisChordType != -1 && chords3.size() > chordScore)
            {
                chordScore = (int)chords3.size();
                key = chords2[i] % 12;
                chordType = thisChordType;
            }
        }

        if (chordType != -1)
        {        
            if (key != m_lastChordKey || chordType != m_lastChordType)
                m_guitarChordPlayer.cc(64, 0);

            m_guitarChordPlayer.cc(64, 127);

            int averageVelocity = 0;
            for (int i = 0; i < chord.size(); ++i)
                if (chord[i].isNoteOn())
                    averageVelocity += chord[i].getVelocity();
            averageVelocity /= chord.size();
            int baseNote = 36;
            m_guitarChordPlayer.noteOn(baseNote + chordType * 12 + key, averageVelocity);
            m_guitarChordPlayer.noteOff(baseNote + chordType * 12 + key); // always noteoff next block, let sustain do the work

                    m_lastChordKey = key;
                    m_lastChordType = chordType;

            for (int i = 0; i < m_buffer.size(); ++i)
                m_buffer[i].clear();
        }
    }
            
    AudioSourceChannelInfo channelInfo;
    channelInfo.buffer = &buffer;
    channelInfo.numSamples = buffer.getNumSamples();
    channelInfo.startSample = 0;
    m_guitarChordPlayer.getNextAudioBlock(channelInfo);

    dsp::Reverb::Parameters parameters;
    const float wetScaleFactor = 3.0f;
    const float dryScaleFactor = 2.0f;
    parameters.wetLevel = 0.5f / wetScaleFactor;
    parameters.dryLevel = 1.f / dryScaleFactor;
    m_reverb->setParameters(parameters);
    auto block = dsp::AudioBlock<float>(buffer);
    dsp::ProcessContextReplacing<float> context(block);
    m_reverb->process(context);
	m_reverbActive = true;

    midiMessages.clear();
}

//==============================================================================
bool GuitarStrummerAudioProcessor::hasEditor() const
{
    return false;
}

AudioProcessorEditor* GuitarStrummerAudioProcessor::createEditor()
{
    return nullptr;
}

//==============================================================================
void GuitarStrummerAudioProcessor::getStateInformation (MemoryBlock& )
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void GuitarStrummerAudioProcessor::setStateInformation (const void* , int )
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}
