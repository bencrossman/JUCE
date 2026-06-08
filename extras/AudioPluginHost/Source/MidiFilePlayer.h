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

    void fillBuffer (int numSamples, int sampleRate, MidiBuffer& output, float performanceTempoBpm, int midiChannel = 1);
    double getFileTempoBpm() const { return fileTempoBpm; }

private:
    MidiMessageSequence sequence;
    double fileTempoBpm = 120.0;
    double endTimeSeconds = 0;
    int nextEventIndex = 0;
    double positionSeconds = 0;
    bool active = false;
};

File resolveMidiFilePath (const String& path);
