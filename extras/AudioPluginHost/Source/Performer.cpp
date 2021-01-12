#include "Performer.h"
#include "ForteEmulator/ForteSDK/ForteTypes.h"

std::string  defaultstring;

void Replace(std::string &result, const std::string& replaceWhat, const std::string& replaceWithWhat)
{
    while (1)
    {
        auto pos = result.find(replaceWhat);
        if (pos == -1) break;
        result.replace(pos, replaceWhat.size(), replaceWithWhat);
    }
}



void TrimRight(std::string& str, const char* chars2remove)
{
    if (!str.empty())
    {
        std::string::size_type pos = str.find_last_not_of(chars2remove);

        if (pos != std::string::npos)
            str.erase(pos + 1);
        else
            str.erase(str.begin(), str.end()); // make empty
    }
}


void Performer::Import(const char *fileToLoad)
{
    //const char *fileToLoad = "D:\\Data\\Audio\\Synth Backups\\Software\\Performance\\Queen.rcf";

    ForteFile file;
    memset(&file, 0, sizeof(ForteFile));
    XmlArchive::Load(fileToLoad, file);
	
	// Devices
	for (auto d = 0U; d < file.Rack.MixerScene[0].Mixer.Group.InputGroup.size(); ++d)
	{
		auto group = file.Rack.MixerScene[0].Mixer.Group.InputGroup[d];
		Device newDevice;
		newDevice.ID = group.ID;
		newDevice.Name = group.Name;
        newDevice.PluginName = group.PluginChain.PlugIn[0].Name;
		if (newDevice.Name == "Arpeggiator")
			continue;

		Root.Racks.Rack.push_back(newDevice);
	}

	// Setlists
	for (unsigned int sl = 0; sl < file.Rack.Setlists.Setlist.size(); ++sl)
	{
		auto &setlist = file.Rack.Setlists.Setlist[sl];
		SetList newSetList;
		newSetList.ID = setlist.ID;
		newSetList.Name = setlist.Name;
		for (unsigned int sr = 0; sr < setlist.SongRef.size(); ++sr)
			newSetList.Song.push_back(Integer(setlist.SongRef[sr].ID));
		Root.SetLists.SetList.push_back(newSetList);
	}

	Root.CurrentSetListID = Root.SetLists.SetList[file.Rack.Setlists.Active].ID;

	m_currentSetlistIndex = file.Rack.Setlists.Active;

    // Performances
	int m_initialID = -1;
    for (unsigned int mi = 0; mi<file.Rack.MixerScene.size(); ++mi)
    {
		auto &mixer = file.Rack.MixerScene[mi];

        if (mixer.Name == "SaveState")
        {
            for (int ig = 0; ig < (int)mixer.Mixer.Group.InputGroup.size(); ++ig)
            {
                auto &blobref = mixer.Mixer.Group.InputGroup[ig].PluginChain.PlugIn[0].SharedBlobRef;
                auto pluginName = mixer.Mixer.Group.InputGroup[ig].PluginChain.PlugIn[0].Name;
                if (pluginName != "TruePianos" &&
					pluginName != "M1" &&
					pluginName != "OP-X PRO-II" && // Forte wont save the entire bank state
					pluginName != "WAVESTATION" &&
					pluginName != "energyXT2 VSTi" &&
					pluginName != "FM7" &&
                    pluginName != "SUPERWAVE P8" &&
                    pluginName != "GuitarStrummer")
                {
                    for (int r = 0; r < (int)Root.Racks.Rack.size(); ++r)
                    {
                        if (Root.Racks.Rack[r].PluginName == mixer.Mixer.Group.InputGroup[ig].PluginChain.PlugIn[0].Name)
                        {
                            auto &blobs = file.Rack.Global.SharedBlobs.SharedBlob;
                            for (unsigned int bi = 0; bi < blobs.size(); ++bi)
                            {
                                if (blobs[bi].Key == blobref.Key)
                                {
                                    Root.Racks.Rack[r].InitialState = blobs[bi].contents;
                                }
                            }
                        }
                    }
                }
            }
        }

		if (mixer.Name == "Initial")
			m_initialID = mixer.ID;

        if (mixer.Name == "SaveState" || mixer.Name == "Initial") // dont need these now
            continue;

        string songName = mixer.Name;
        TrimRight(songName);

        PerformanceType performance;
        performance.ID = mixer.ID;
        performance.Name = songName;
        performance.Tempo = mixer.Mixer.Tempo.BPM;
        for (int ig = 0; ig<(int)mixer.Mixer.Group.InputGroup.size(); ++ig)
        {
			auto &group = mixer.Mixer.Group.InputGroup[ig];
            if (!group.Mute)
            {
				auto &onSetScene = group.PluginChain.PlugIn[0].OnSetScene;

				for (auto pass = 0U; pass < 2; ++pass)
				{
					if (pass == 1 && onSetScene.ProgramChange.size() <= 1)
						continue; 

					Zone zone;
					zone.Solo = false;
					zone.Mute = false;
					zone.NoteMode = NoteMode::Normal;
					zone.Volume = group.Gain;
					zone.Device = NULL;
					zone.DeviceID = group.ID + pass * 3;

					auto &midiFilters = group.PluginChain.PlugIn[0].MIDIFilterSet.MIDIFilter;
					ForteMIDIFilter filter;
					if (midiFilters.size() > 0)
						filter = midiFilters[0];

					if (onSetScene.ProgramChange.size() <= 1 && filter.MapChannel.size() == 2 && abs(filter.MapChannel[0].Key.Transpose - filter.MapChannel[0].Key.Transpose) == 12) // see if this is a transpose
					{
						zone.NoteMode = NoteMode::DoubleOctave;
						zone.Transpose = filter.MapChannel[0].Key.Transpose < filter.MapChannel[1].Key.Transpose ? filter.MapChannel[0].Key.Transpose : filter.MapChannel[1].Key.Transpose;
					}
					else
						zone.Transpose = (pass < filter.MapChannel.size()) ? filter.MapChannel[pass].Key.Transpose : 0;

					zone.LowKey = (pass < filter.MapChannel.size()) ? filter.MapChannel[pass].Key.Low : 0;
					zone.HighKey = (pass < filter.MapChannel.size()) ? filter.MapChannel[pass].Key.High : 127;
				
					if (pass < onSetScene.ProgramChange.size())
					{
						zone.Program = onSetScene.ProgramChange[pass].Program;
						zone.Bank = onSetScene.ProgramChange[pass].Bank;
					}
					else if (group.Name != "Arpeggiator")
					{
						zone.Program = -1;
						zone.Bank = -1;
					}

					if (group.PluginChain.PlugIn[0].MIDIFilterSet.vMIDIFilter.size())
					{
						auto &filter2 = group.PluginChain.PlugIn[0].MIDIFilterSet.vMIDIFilter[0]; // should be only one
						if (!filter2.Disabled && filter2.Name.find("Arpeggiator") != -1)
							zone.NoteMode = NoteMode::ThreeOctaveArpeggio;
						
					}

					if (group.Name == "Arpeggiator")
					{
						// copy parameters to other plugin
						for (auto z = 0U; z<performance.Zone.size(); ++z)
							if (performance.Zone[z].NoteMode == NoteMode::ThreeOctaveArpeggio)
							{
								performance.Zone[z].LowKey = zone.LowKey;
								performance.Zone[z].HighKey = zone.HighKey;
								performance.Zone[z].Transpose = zone.Transpose;
							}
						continue;
					}

					performance.Zone.push_back(zone);

				}
            }
        }
        Root.Performances.Performance.push_back(performance);
    }


	// Songs
	for (unsigned int si = 0; si < file.Rack.Setlists.Song.size(); ++si)
	{
		auto &song = file.Rack.Setlists.Song[si];
		Song newSong;
		newSong.ID = song.ID;
		newSong.Name = song.Name;
		for (unsigned int mr = 0; mr < song.MixerSceneRef.size(); ++mr)
			if (song.MixerSceneRef[mr].ID != m_initialID)

				newSong.Performance.push_back(song.MixerSceneRef[mr].ID);
		Root.Songs.Song.push_back(newSong);
	}


}

void Performer::ResolveIDs()
{
	// Resolve current setlist
	for (auto sl = 0U; sl < Root.SetLists.SetList.size(); ++sl)
		if (Root.SetLists.SetList[sl].ID == Root.CurrentSetListID)
			m_currentSetlistIndex = sl;

    // Resolve songs in setlists
    for (auto sl = 0U; sl < Root.SetLists.SetList.size(); ++sl)
        for (auto sg = 0U; sg < Root.SetLists.SetList[sl].Song.size(); ++sg)
            for (auto i = 0U; i < Root.Songs.Song.size(); ++i)
                if (Root.SetLists.SetList[sl].Song[sg].ID == Root.Songs.Song[i].ID)
                    Root.SetLists.SetList[sl].SongPtr.push_back(&Root.Songs.Song[i]);

    // Resolve performances in songs
    for (auto sg = 0U; sg < Root.Songs.Song.size(); ++sg)
        for (auto p = 0U; p < Root.Songs.Song[sg].Performance.size(); ++p)
            for (auto i = 0U; i < Root.Performances.Performance.size(); ++i)
                if (Root.Songs.Song[sg].Performance[p].ID == Root.Performances.Performance[i].ID)
                    Root.Songs.Song[sg].PerformancePtr.push_back(&Root.Performances.Performance[i]);

    for (auto p = 0U; p < Root.Performances.Performance.size(); ++p)
    {
        // Add muted zones for other devices
        for (auto d = 0U; d < Root.Racks.Rack.size(); ++d)
        {
            bool found = false;
            for (auto z = 0U; z < Root.Performances.Performance[p].Zone.size(); ++z)
                if (Root.Performances.Performance[p].Zone[z].DeviceID == Root.Racks.Rack[d].ID)
                    found = true;

            if (!found)
            {
                Zone mutedZone;
                XmlArchive ar(NULL);
                mutedZone.Serialize(ar); // initialize to defaults
                mutedZone.DeviceID = Root.Racks.Rack[d].ID;
                mutedZone.Mute = true;
                Root.Performances.Performance[p].Zone.push_back(mutedZone);
            }
        }

        // Resolve devices in performances
        for (auto z = 0U; z < Root.Performances.Performance[p].Zone.size(); ++z)
        {
            for (auto d = 0U; d < Root.Racks.Rack.size(); ++d)
                if (Root.Performances.Performance[p].Zone[z].DeviceID == Root.Racks.Rack[d].ID)
                    Root.Performances.Performance[p].Zone[z].Device = &Root.Racks.Rack[d];
            if (Root.Performances.Performance[p].Zone[z].Device == NULL)
            {
                swap(Root.Performances.Performance[p].Zone[z], Root.Performances.Performance[p].Zone.back());
                Root.Performances.Performance[p].Zone.pop_back();
                z--;
            }
        }
    }
}

int Performer::GetTotalPerformances()
{
    int performanceIndices = 0;
    auto &setlist = Root.SetLists.SetList[m_currentSetlistIndex];
    for (int s = 0; s < setlist.SongPtr.size(); ++s)
        for (int p = 0; p < setlist.SongPtr[s]->PerformancePtr.size(); ++p)
            performanceIndices++;
    return performanceIndices;
}

void Performer::GetPerformanceByIndex(PerformanceType*&performance, Song *&song, int index)
{ 
    auto totalPerformances = GetTotalPerformances();
    if (totalPerformances > 0)
        index = index % totalPerformances;
    else
        index = 0;
    auto &setlist = Root.SetLists.SetList[m_currentSetlistIndex];
    int count=0;
    bool found = false;
    for (int s = 0; s < setlist.SongPtr.size(); ++s)
    {
        for (int p = 0; p < setlist.SongPtr[s]->PerformancePtr.size(); ++p)
        {
            if (count == index)
            {
                found = true;
                performance = setlist.SongPtr[s]->PerformancePtr[p];
                song = setlist.SongPtr[s];
                break;
            }
            count++;
        }
        if (found)
            break;
    }
}

int Performer::GetIndexByPerformance(int songID, int performanceID)
{
	auto &setlist = Root.SetLists.SetList[m_currentSetlistIndex];
	int count = 0;
	for (int s = 0; s < setlist.SongPtr.size(); ++s)
	{
		for (int p = 0; p < setlist.SongPtr[s]->PerformancePtr.size(); ++p)
		{
			if ((songID == -1 || setlist.Song[s].ID == songID) && setlist.SongPtr[s]->PerformancePtr[p]->ID == performanceID)
				return count;
			count++;
		}
	}
	return -1;
}