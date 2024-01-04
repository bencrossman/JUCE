/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"




//==============================================================================
/**
*/
class SoundFontPlayerAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    SoundFontPlayerAudioProcessor();
    ~SoundFontPlayerAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;
    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void Play();
    void Stop();
    std::vector<std::string> *GetList() { return &m_patches; }
	
private:
    ResamplingAudioSource *m_resamplerSource;
    std::vector<std::string> m_patches;
    int m_currentFile;
	int m_samplesPerBlock;
	AudioFormatManager m_formatManager;
    bool m_triggered = false;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoundFontPlayerAudioProcessor)
};

