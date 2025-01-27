#include "SoundfontAudioSource.h"

//==============================================================================
//==============================================================================
SoundfontAudioSource::SoundfontAudioSource(int numberOfVoices)
{
    settings = new_fluid_settings();

    fluid_settings_setstr(settings, "synth.reverb.active", "no"); // in-built reverb sucks
    fluid_settings_setstr(settings, "synth.chorus.active", "no");

    synth = new_fluid_synth(settings);
    
    fluid_synth_set_polyphony(synth, numberOfVoices);
    setGain(1.0f);
}

SoundfontAudioSource::~SoundfontAudioSource()
{
    delete_fluid_synth(synth);
    delete_fluid_settings(settings);
}

void SoundfontAudioSource::prepareToPlay(int samplesPerBlock, double sampleRate)
{
    fluid_synth_set_sample_rate(synth, (float) sampleRate);

    m_reverb->prepareToPlay(sampleRate, samplesPerBlock);
}

void SoundfontAudioSource::releaseResources()
{
    systemReset();
}

void SoundfontAudioSource::process_bypassed()
{
    if (m_reverbActive)
    {
        m_reverb->reset();
        m_reverbActive = false;
    }
}

void SoundfontAudioSource::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    // Remove any sounds coming in
    bufferToFill.clearActiveBufferRegion();
    
    // Prevent loading/unloading soundfonts & stuff while writing floats.
    // That is how you get race conditions and crashes.
    const ScopedLock l (lock);
    
    fluid_synth_write_float(synth,
                            bufferToFill.numSamples,
                            bufferToFill.buffer->getWritePointer(0), 0, 1,
                            bufferToFill.buffer->getWritePointer(1), 0, 1);

    if (fluid_synth_get_reverb(synth) > 0) // This is set on start of a voice so will switch on different samples, in-built reverb must be not working
    {
        MidiBuffer midiBuffer;
        m_reverb->processBlock(*bufferToFill.buffer, midiBuffer); // Just use standard level, not a simple wet value unfortunately
        m_reverbActive = true;
    }
}

bool SoundfontAudioSource::loadSoundfont(const File file)
{
    if (file == loadedSoundfont) {
        // Don't reload an already loaded soundfont
        return false;
    }

    if (!file.exists())
        return false;

    loadedSoundfont = file;
    
    // Lock while switching soundfonts
    const ScopedLock l (lock);
    
    // All notes off
    fluid_synth_system_reset(synth);
    
    // If a soundfont is already loaded, unload the previous one (this demo doesn't go into banks, etc).
    if (fluid_synth_sfcount(synth) > 0) {
        int err = fluid_synth_sfunload(synth, (unsigned int) sfontID, true);
        if (err == -1) {
            return false;
        }
    }
    
    // Load the soundfont, store the handle
    sfontID = fluid_synth_sfload(synth, file.getFullPathName().toRawUTF8(), true);
    return sfontID != -1;
}

void SoundfontAudioSource::processMidi (const MidiMessage& message)
{
    if (message.isNoteOn()) {
        noteOn(message.getNoteNumber(), message.getVelocity());
    }
    else if (message.isNoteOff()) {
        noteOff(message.getNoteNumber());
    }
    else if (message.isController()) {
        cc(message.getControllerNumber(), message.getControllerValue());
    }
    else if (message.isPitchWheel()) {
        pitchBend(message.getPitchWheelValue());
    }
    else if (message.isChannelPressure()) {
        channelPressure(message.getChannelPressureValue());
    }
    // Add support for other types of MIDI messages here
}

void SoundfontAudioSource::noteOn (int note, int velocity, int channel)
{
    // Actually do note off if the velocity is 0
    velocity == 0
        ? fluid_synth_noteoff(synth, channel, note)
        : fluid_synth_noteon(synth, channel, note, velocity);
}

void SoundfontAudioSource::noteOff (int note, int channel)
{
    fluid_synth_noteoff(synth, channel, note);
}

void SoundfontAudioSource::cc(int control, int value, int channel)
{
    fluid_synth_cc(synth, channel, control, value);
}

int SoundfontAudioSource::getCc(int control, int channel)
{
    int value = 0;
    fluid_synth_get_cc(synth, channel, control, &value);
    return value;
}

void SoundfontAudioSource::pitchBend(int value, int channel)
{
    fluid_synth_pitch_bend(synth, channel, value);
}

int SoundfontAudioSource::getPitchBend(int channel)
{
    int value = 0;
    fluid_synth_get_pitch_bend(synth, channel, &value);
    return value;
}

void SoundfontAudioSource::setPitchBendRange(int value, int channel)
{
    fluid_synth_pitch_wheel_sens(synth, channel, value);
}

int SoundfontAudioSource::getPitchBendRange(int channel)
{
    int value = 0;
    fluid_synth_get_pitch_wheel_sens(synth, channel, &value);
    return value;
}

void SoundfontAudioSource::channelPressure(int value, int channel)
{
    fluid_synth_channel_pressure(synth, channel, value);
}

void SoundfontAudioSource::setGain (float gain)
{
    fluid_synth_set_gain(synth, gain);
}

float SoundfontAudioSource::getGain()
{
    return fluid_synth_get_gain(synth);
}

void SoundfontAudioSource::systemReset()
{
    fluid_synth_system_reset(synth);
}

