#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class MidiFilePlayer
{
public:
    MidiFilePlayer() = default;
    MidiFilePlayer (const MidiFilePlayer&) = default;
    MidiFilePlayer& operator= (const MidiFilePlayer&) = default;
    MidiFilePlayer (MidiFilePlayer&&) noexcept = default;
    MidiFilePlayer& operator= (MidiFilePlayer&&) noexcept = default;

    bool load (const File& file);
    void start();
    void stop();
    bool isActive() const { return active; }

    void fillBuffer (int numSamples, int sampleRate, MidiBuffer& output, int midiChannel = 1);

private:
    MidiMessageSequence sequence;
    double endTimeSeconds = 0;
    int nextEventIndex = 0;
    double positionSeconds = 0;
    bool active = false;
};

File resolveMidiFilePath (const String& path);
