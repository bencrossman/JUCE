/*
  ==============================================================================

   This file is part of the JUCE framework examples.
   Copyright (c) Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   to use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
   REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
   AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
   INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
   LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
   OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
   PERFORMANCE OF THIS SOFTWARE.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             GainPlugin
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Gain audio plugin.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
                   juce_audio_plugin_client, juce_audio_processors,
                   juce_audio_utils, juce_core, juce_data_structures,
                   juce_events, juce_graphics, juce_gui_basics, juce_gui_extra
 exporters:        xcode_mac, vs2022

 moduleFlags:      JUCE_STRICT_REFCOUNTEDPOINTER=1

 type:             AudioProcessor
 mainClass:        GainProcessor

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/

#pragma once


//==============================================================================
class GainProcessor final : public AudioProcessor
{
public:

    //==============================================================================
    GainProcessor()
        : AudioProcessor (BusesProperties().withInput  ("Input",  AudioChannelSet::stereo())
                                           .withOutput ("Output", AudioChannelSet::stereo()))
    {
        addParameter (gain = new AudioParameterFloat ({ "gain", 1 }, "Gain", 0.0f, 1.0f, 0.25f));
        addParameter(m_mixDownMode = new AudioParameterFloat("mono", "Mono", 0.f, 1.f, 0.f));
        addParameter(m_analyse = new AudioParameterFloat("analyse", "Analyse", 0.f, 1.f, 0.f));
        
	}

    //==============================================================================
    void prepareToPlay (double, int) override {}
    void releaseResources() override {}

    void processBlock (AudioBuffer<float>& buffer, MidiBuffer&) override
    {
        buffer.applyGain (*gain * 4);

		if (m_mixDownMode->get() == 0.5f)
		{
			buffer.applyGain(0.5f);
			buffer.addFrom(0, 0, buffer, 1, 0, buffer.getNumSamples()); // mix
			buffer.copyFrom(1, 0, buffer, 0, 0, buffer.getNumSamples()); // Copy left to right channel
		}
		if (m_mixDownMode->get() == 1.f)
		{
			buffer.copyFrom(1, 0, buffer, 0, 0, buffer.getNumSamples()); // Copy left to right channel
		}
        if (m_analyse->get() > 0)
            m_quiet = buffer.getMagnitude(0, buffer.getNumSamples()) < 0.001;

    }

    void processBlock (AudioBuffer<double>& buffer, MidiBuffer&) override
    {
        buffer.applyGain ((float) *gain * 4);

		if (m_mixDownMode->get() == 0.5f)
		{
			buffer.applyGain(0.5f);
			buffer.addFrom(0, 0, buffer, 1, 0, buffer.getNumSamples()); // mix
			buffer.copyFrom(1, 0, buffer, 0, 0, buffer.getNumSamples()); // Copy left to right channel
		}
		if (m_mixDownMode->get() == 1.f)
		{
			buffer.copyFrom(1, 0, buffer, 0, 0, buffer.getNumSamples()); // Copy left to right channel
		}
        if (m_analyse->get() > 0)
            m_quiet = buffer.getMagnitude(0, buffer.getNumSamples()) < 0.001;
    }

    //==============================================================================
    AudioProcessorEditor* createEditor() override          { return new GenericAudioProcessorEditor (*this); }
    bool hasEditor() const override                        { return true;   }

    //==============================================================================
    const String getName() const override                  { return "Gain PlugIn"; }
    bool acceptsMidi() const override                      { return false; }
    bool producesMidi() const override                     { return false; }
    double getTailLengthSeconds() const override           
    { 
        return m_quiet ? 0 : 1; 
    }

    //==============================================================================
    int getNumPrograms() override                          { return 1; }
    int getCurrentProgram() override                       { return 0; }
    void setCurrentProgram (int) override                  {}
    const String getProgramName (int) override             { return "None"; }
    void changeProgramName (int, const String&) override   {}

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override
    {
        MemoryOutputStream (destData, true).writeFloat (*gain);
    }

    void setStateInformation (const void* data, int sizeInBytes) override
    {
        gain->setValueNotifyingHost (MemoryInputStream (data, static_cast<size_t> (sizeInBytes), false).readFloat());
    }

    //==============================================================================
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override
    {
        const auto& mainInLayout  = layouts.getChannelSet (true,  0);
        const auto& mainOutLayout = layouts.getChannelSet (false, 0);

        return (mainInLayout == mainOutLayout && (! mainInLayout.isDisabled()));
    }

private:
    //==============================================================================
    AudioParameterFloat* gain;
    AudioParameterFloat* m_mixDownMode;
    AudioParameterFloat* m_analyse;

    bool m_quiet = true;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainProcessor)
};
