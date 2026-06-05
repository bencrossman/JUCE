#include "MidiFilePlayer.h"

File resolveMidiFilePath (const String& path)
{
    if (path.isEmpty())
        return {};

    return File::getCurrentWorkingDirectory().getChildFile (path);
}

bool MidiFilePlayer::load (const File& file)
{
    stop();
    sequence.clear();
    endTimeSeconds = 0;

    if (! file.existsAsFile())
        return false;

    FileInputStream stream (file);
    if (! stream.openedOk())
        return false;

    MidiFile midiFile;
    if (! midiFile.readFrom (stream, true))
        return false;

    midiFile.convertTimestampTicksToSeconds();

    for (int t = 0; t < midiFile.getNumTracks(); ++t)
        sequence.addSequence (*midiFile.getTrack (t), 0.0);

    sequence.updateMatchedPairs();
    endTimeSeconds = sequence.getEndTime();

    return sequence.getNumEvents() > 0;
}

void MidiFilePlayer::start()
{
    nextEventIndex = 0;
    positionSeconds = 0;
    active = sequence.getNumEvents() > 0;
}

void MidiFilePlayer::stop()
{
    active = false;
    nextEventIndex = 0;
    positionSeconds = 0;
}

void MidiFilePlayer::fillBuffer (int numSamples, int sampleRate, MidiBuffer& output, int midiChannel)
{
    if (! active || sampleRate <= 0)
        return;

    const double blockStart = positionSeconds;
    const double blockEnd = blockStart + numSamples / (double) sampleRate;

    while (nextEventIndex < sequence.getNumEvents())
    {
        const auto eventTime = sequence.getEventTime (nextEventIndex);

        if (eventTime >= blockEnd)
            break;

        if (eventTime >= blockStart)
        {
            auto message = sequence.getEventPointer (nextEventIndex)->message;
            message.setChannel (midiChannel);
            message.setTimeStamp (0);

            const int samplePos = jlimit (0, numSamples - 1,
                                            (int) std::round ((eventTime - blockStart) * sampleRate));
            output.addEvent (message, samplePos);
        }

        ++nextEventIndex;
    }

    positionSeconds = blockEnd;

    if (nextEventIndex >= sequence.getNumEvents() || positionSeconds >= endTimeSeconds)
        active = false;
}
