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

    fileTempoBpm = 120.0;
    MidiMessageSequence tempoEvents;
    midiFile.findAllTempoEvents (tempoEvents);

    if (tempoEvents.getNumEvents() > 0)
    {
        const auto& msg = tempoEvents.getEventPointer (0)->message;

        if (msg.isTempoMetaEvent())
            fileTempoBpm = 60.0 / msg.getTempoSecondsPerQuarterNote();
    }

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

void MidiFilePlayer::fillBuffer (int numSamples, int sampleRate, MidiBuffer& output, float performanceTempoBpm, int midiChannel)
{
    if (! active || sampleRate <= 0)
        return;

    if (performanceTempoBpm <= 0.0f)
        performanceTempoBpm = 120.0f;

    const double tempoScale = performanceTempoBpm / fileTempoBpm;
    const double wallDelta = numSamples / (double) sampleRate;
    const double fileDelta = wallDelta * tempoScale;
    const double blockStart = positionSeconds;
    const double blockEnd = blockStart + fileDelta;

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
                                            (int) std::round ((eventTime - blockStart) / fileDelta * numSamples));
            output.addEvent (message, samplePos);
        }

        ++nextEventIndex;
    }

    positionSeconds = blockEnd;

    if (nextEventIndex >= sequence.getNumEvents() || positionSeconds >= endTimeSeconds)
        active = false;
}
