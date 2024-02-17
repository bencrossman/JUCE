/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "SoundFontPlayerProcessor.h"
#include "SoundFontPlayerEditor.h"

//==============================================================================
SoundFontPlayerAudioProcessor::SoundFontPlayerAudioProcessor() : AudioProcessor (BusesProperties().withOutput ("Output", AudioChannelSet::stereo(), true))
{
    m_patches.resize(128);
    m_players.resize(128);
    for (int i = 0; i < 128; ++i)
        m_players[i] = nullptr;
    m_currentFile=0;
}

SoundFontPlayerAudioProcessor::~SoundFontPlayerAudioProcessor()
{
    for (int i = 0; i < 128; ++i)
        delete m_players[i];
}

//==============================================================================
const String SoundFontPlayerAudioProcessor::getName() const
{
    return "Sound Font Player";
}

bool SoundFontPlayerAudioProcessor::acceptsMidi() const
{
    return true;
}

bool SoundFontPlayerAudioProcessor::producesMidi() const
{
    return false;
}

bool SoundFontPlayerAudioProcessor::isMidiEffect() const
{
    return false;
}

double SoundFontPlayerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SoundFontPlayerAudioProcessor::getNumPrograms()
{
    return 128;
}

int SoundFontPlayerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SoundFontPlayerAudioProcessor::setCurrentProgram (int)
{
}

const String SoundFontPlayerAudioProcessor::getProgramName (int index)
{
    std::string res;
    if (m_patches[index].m_file != "")
    {
        res = m_patches[index].m_file;
        if(const char *found=strrchr(m_patches[index].m_file.c_str(),'\\'))
            res = found+1;
        if(const char *found=strrchr(m_patches[index].m_file.c_str(),'/'))
            res = found+1;
        res.resize(res.size()-4);
    }
    return res;
}


//==============================================================================
void SoundFontPlayerAudioProcessor::prepareToPlay (double sampleRate, int)
{
    m_sampleRate = sampleRate;

    m_reverb.reset(new dsp::Reverb());
}

void SoundFontPlayerAudioProcessor::releaseResources()
{
}


bool SoundFontPlayerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    return true;
}

void SoundFontPlayerAudioProcessor::processBlockBypassed(AudioBuffer<float>&buffer, MidiBuffer& midiMessages)
{
    AudioProcessor::processBlockBypassed(buffer, midiMessages);
    
    if (m_players[m_currentFile])
        m_players[m_currentFile]->process_bypassed();
}

void SoundFontPlayerAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    if (m_loading)
        return;

    if (m_patches[m_currentFile].m_file == "" && m_players[m_currentFile])
    {
        delete m_players[m_currentFile];
        m_players[m_currentFile] = nullptr;
    }


    if (m_patches[m_currentFile].m_file != "" && m_patches[m_currentFile].m_reload)
    {
        m_patches[m_currentFile].m_reload = false;
        delete m_players[m_currentFile];
        m_players[m_currentFile] = new SoundfontAudioSource();
        m_players[m_currentFile]->loadSoundfont(File(m_patches[m_currentFile].m_file));
        m_players[m_currentFile]->SetReverb(m_reverb.get());
        m_players[m_currentFile]->prepareToPlay(buffer.getNumSamples(), m_sampleRate);
    }


    for (const auto meta : midiMessages)
    {
        if (meta.getMessage().isProgramChange())
        {
            m_currentFile = meta.getMessage().getProgramChangeNumber();
            if (m_players[m_currentFile])
            {
                m_players[m_currentFile]->SetReverb(m_reverb.get());
                m_players[m_currentFile]->prepareToPlay(buffer.getNumSamples(), m_sampleRate);
            }
        }
        else
        {
            if (m_players[m_currentFile])
                m_players[m_currentFile]->processMidi(meta.getMessage());
        }
    }

	AudioSourceChannelInfo sourceChannelInfo;
	sourceChannelInfo.buffer = &buffer;
	sourceChannelInfo.startSample = 0;
	sourceChannelInfo.numSamples = buffer.getNumSamples();
    if (m_players[m_currentFile])
	    m_players[m_currentFile]->getNextAudioBlock(sourceChannelInfo);
}


void SoundFontPlayerAudioProcessor::changeProgramName (int , const String& )
{
}



//==============================================================================
bool SoundFontPlayerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SoundFontPlayerAudioProcessor::createEditor()
{
    return new SoundFontPlayerEditor (*this);
}

//==============================================================================
void SoundFontPlayerAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    std::vector<uint8> data;
    data.push_back(m_preload);
    for(int i=0;i<(int)m_patches.size();++i)
    {
        data.push_back((uint8)m_patches[i].m_file.size());
        for(int j=0;j<(int)m_patches[i].m_file.size();++j)
            data.push_back(m_patches[i].m_file[j]);
    }
    destData.setSize(data.size());
    destData.copyFrom(data.data(),0,data.size());
}

void SoundFontPlayerAudioProcessor::setStateInformation (const void* data, int )
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    uint8*ptr=(uint8*)data;
    m_preload = *ptr++;
    for(int i=0;i<(int)m_patches.size();++i)
    {
        m_patches[i].m_file.resize(*ptr++);
        for(int j=0;j<(int)m_patches[i].m_file.size();++j)
            m_patches[i].m_file[j] =*ptr++;
    }

    if (!m_preload)
        return;

    m_loading = true;
    for (int i = 0; i < (int)m_patches.size(); ++i)
    {
        if (m_patches[i].m_file != "")
        {
            m_players[i] = new SoundfontAudioSource();
            m_players[i]->loadSoundfont(File(m_patches[i].m_file));
        }
    }
    m_loading = false;
}
