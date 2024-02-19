/**
 *  ElephantDSP.com Room Reverb
 *
 *  Copyright (C) 2021 Christian Voigt
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PluginProcessor.h"


//==============================================================================
void ReverbAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused(samplesPerBlock);
    mRoomReverb.setSampleRate(static_cast<float>(sampleRate));
}

void ReverbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    mRoomReverb.mute();
}


void ReverbAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                        juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    if (totalNumInputChannels == 1 && totalNumOutputChannels == 2)
    {
        // mono in, stereo out
        mRoomReverb.process(buffer.getReadPointer(0), buffer.getReadPointer(0),
                            buffer.getWritePointer(0), buffer.getWritePointer(1),
                            buffer.getNumSamples());
    }
    else if (totalNumInputChannels == 2 && totalNumOutputChannels == 2)
    {
        // stereo in, stereo out
        mRoomReverb.process(buffer.getReadPointer(0), buffer.getReadPointer(1),
                            buffer.getWritePointer(0), buffer.getWritePointer(1),
                            buffer.getNumSamples());
    }
    else
    {
        jassertfalse; // channel layout not supported
    }
}
