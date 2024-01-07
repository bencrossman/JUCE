/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "../GuitarStrummer/SoundfontAudioSource.h"


//==============================================================================
/**
*/
class SoundFontPlayerAudioProcessor  : public AudioProcessor
{
public:

    struct Patch
    {
        std::string m_file;
        bool m_reload = false;
    };

    //==============================================================================
    SoundFontPlayerAudioProcessor();
    ~SoundFontPlayerAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;
    void processBlockBypassed(AudioBuffer<float>& buffer, MidiBuffer& midiMessages);
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

    std::vector<Patch> *GetList() { return &m_patches; }
    bool* GetPreload() { return &m_preload; }
	
private:
    std::vector<Patch> m_patches;
    std::vector<SoundfontAudioSource*> m_players;
    int m_currentFile;
    double m_sampleRate;
    bool m_loading = false;
    bool m_preload = true;

    std::shared_ptr<dsp::Reverb> m_reverb;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoundFontPlayerAudioProcessor)
};

