/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "WavStreamerProcessor.h"
#include "WavStreamerEditor.h"

//==============================================================================
WavStreamerAudioProcessor::WavStreamerAudioProcessor() : AudioProcessor (BusesProperties().withOutput ("Output", AudioChannelSet::stereo(), true))
{
  m_patches.resize(127);
  m_currentFile=0;
  m_resamplerSource = NULL;
  m_formatManager.registerBasicFormats();
}

WavStreamerAudioProcessor::~WavStreamerAudioProcessor()
{
    Stop();
}

//==============================================================================
const String WavStreamerAudioProcessor::getName() const
{
    return "Wav Streamer";
}

bool WavStreamerAudioProcessor::acceptsMidi() const
{
    return true;
}

bool WavStreamerAudioProcessor::producesMidi() const
{
    return false;
}

bool WavStreamerAudioProcessor::isMidiEffect() const
{
    return false;
}

double WavStreamerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WavStreamerAudioProcessor::getNumPrograms()
{
    return 127;
}

int WavStreamerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WavStreamerAudioProcessor::setCurrentProgram (int)
{
}

const String WavStreamerAudioProcessor::getProgramName (int index)
{
  if (index==0)
    return "Disabled";

  std::string res;
  if (m_patches[index - 1].m_file != "")
  {
    res = m_patches[index-1].m_file;
    if(const char *found=strrchr(m_patches[index-1].m_file.c_str(),'\\'))
      res = found+1;
    res.resize(res.size()-4);
  }

  return res;
}


//==============================================================================
void WavStreamerAudioProcessor::prepareToPlay (double , int samplesPerBlock)
{
	m_samplesPerBlock = samplesPerBlock;
}

void WavStreamerAudioProcessor::releaseResources()
{
}


bool WavStreamerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    return true;
}


void WavStreamerAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    //midi input part
    if (!midiMessages.isEmpty() && m_patches.size())
    {
        for (const auto meta : midiMessages)
        {
            const uint8 *b= meta.getMessage().getRawData();
            if ((b[0]&0xf0)==0xc0) // program change
            {
                Stop();

                if (b[1]>0 && b[1]-1<(int)m_patches.size())
                {
                    m_currentFile=b[1]-1;
                    bool onNote = m_patches[m_currentFile].m_mode == MODE_ONNOTE;
                    if (onNote)
                        m_triggered = false;
                    else
                        Play();
                }
            }

            bool onNote = m_patches[m_currentFile].m_mode == MODE_ONNOTE;
            if (meta.getMessage().isNoteOn() && !m_triggered && onNote)
            {
                m_triggered = true;
                Play();
            }
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


void WavStreamerAudioProcessor::changeProgramName (int , const String& )
{
}



//==============================================================================
bool WavStreamerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* WavStreamerAudioProcessor::createEditor()
{
    return new WavStreamerEditor (*this);
}

//==============================================================================
void WavStreamerAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
  std::vector<uint8> data;
  for(int i=0;i<(int)m_patches.size();++i)
  {
    data.push_back((uint8)m_patches[i].m_mode);
    data.push_back((uint8)m_patches[i].m_file.size());
    for(int j=0;j<(int)m_patches[i].m_file.size();++j)
      data.push_back(m_patches[i].m_file[j]);
  }
  destData.setSize(data.size());
  destData.copyFrom(data.data(),0,data.size());
}

void WavStreamerAudioProcessor::setStateInformation (const void* data, int )
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
  uint8*ptr=(uint8*)data;
  for(int i=0;i<(int)m_patches.size();++i)
  {
    m_patches[i].m_mode = (*ptr++);
    m_patches[i].m_file.resize(*ptr++);
    for(int j=0;j<(int)m_patches[i].m_file.size();++j)
      m_patches[i].m_file[j]=*ptr++;
  }
}

void WavStreamerAudioProcessor::Play()
{
	File audioFile(m_patches[m_currentFile].m_file);
	AudioFormatReader* reader = m_formatManager.createReaderFor(audioFile);
	if (reader != nullptr)
	{
		auto fileSource = new AudioFormatReaderSource(reader, true);
		fileSource->setLooping(m_patches[m_currentFile].m_mode == MODE_LOOP);

		m_resamplerSource = new ResamplingAudioSource(fileSource, true);
		m_resamplerSource->setResamplingRatio(reader->sampleRate / getSampleRate());
		m_resamplerSource->prepareToPlay(m_samplesPerBlock, getSampleRate());
	}
}

void WavStreamerAudioProcessor::Stop()
{
  if (m_resamplerSource)
  {
	  delete m_resamplerSource;
	  m_resamplerSource = nullptr;
  }
}
