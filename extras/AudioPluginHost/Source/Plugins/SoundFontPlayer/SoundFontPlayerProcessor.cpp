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
  m_patches.resize(127);
  m_currentFile=0;
  m_resamplerSource = NULL;
  m_formatManager.registerBasicFormats();
}

SoundFontPlayerAudioProcessor::~SoundFontPlayerAudioProcessor()
{
    Stop();
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
    return 127;
}

int SoundFontPlayerAudioProcessor::getCurrentProgram()
{
    return m_currentFile;
}

void SoundFontPlayerAudioProcessor::setCurrentProgram (int index)
{
  Stop();
  if (index>0 && index<(int)m_patches.size()+1)
    m_currentFile=index-1;
}

const String SoundFontPlayerAudioProcessor::getProgramName (int index)
{
  if (index==0)
    return "Disabled";

  std::string res;
  if (m_patches[index - 1] != "")
  {
    res = m_patches[index-1];
    if(const char *found=strrchr(m_patches[index-1].c_str(),'\\'))
      res = found+1;
    res.resize(res.size()-4);
  }

  return res;
}


//==============================================================================
void SoundFontPlayerAudioProcessor::prepareToPlay (double , int samplesPerBlock)
{
	m_samplesPerBlock = samplesPerBlock;
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


void SoundFontPlayerAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    //midi input part
    if (!midiMessages.isEmpty() && m_patches.size())
    {
        MidiMessage midi_message(0xf0);
        MidiBuffer output;
        int sample_number;

        MidiBuffer::Iterator midi_buffer_iter(midiMessages);
        while(midi_buffer_iter.getNextEvent(midi_message,sample_number))
        {
        }
    }

    midiMessages.clear();

	if (m_resamplerSource)
	{
		AudioSourceChannelInfo sourceChannelInfo;
		sourceChannelInfo.buffer = &buffer;
		sourceChannelInfo.startSample = 0;
		sourceChannelInfo.numSamples = buffer.getNumSamples();
		m_resamplerSource->getNextAudioBlock(sourceChannelInfo);
	}
	else
        buffer.clear();
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
  for(int i=0;i<(int)m_patches.size();++i)
  {
    data.push_back((uint8)m_patches[i].size());
    for(int j=0;j<(int)m_patches[i].size();++j)
      data.push_back(m_patches[i][j]);
  }
  destData.setSize(data.size());
  destData.copyFrom(data.data(),0,data.size());
}

void SoundFontPlayerAudioProcessor::setStateInformation (const void* data, int )
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
  uint8*ptr=(uint8*)data;
  for(int i=0;i<(int)m_patches.size();++i)
  {
    m_patches[i].resize(*ptr++);
    for(int j=0;j<(int)m_patches[i].size();++j)
      m_patches[i][j]=*ptr++;
  }
}

void SoundFontPlayerAudioProcessor::Play()
{
	File audioFile(m_patches[m_currentFile]);
	AudioFormatReader* reader = m_formatManager.createReaderFor(audioFile);
	if (reader != nullptr)
	{
		auto fileSource = new AudioFormatReaderSource(reader, true);

		m_resamplerSource = new ResamplingAudioSource(fileSource, true);
		m_resamplerSource->setResamplingRatio(reader->sampleRate / getSampleRate());
		m_resamplerSource->prepareToPlay(m_samplesPerBlock, getSampleRate());
	}
}

void SoundFontPlayerAudioProcessor::Stop()
{
  if (m_resamplerSource)
  {
	  delete m_resamplerSource;
	  m_resamplerSource = nullptr;
  }
}
