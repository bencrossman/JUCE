#pragma once

#include "../JuceLibraryCode/JuceHeader.h" // For Uuid

#include <string>
#include <vector>
#include "ForteEmulator/ForteSDK/XmlArchive.h"

using namespace std;

extern void TrimRight(std::string& str, const char* chars2remove = " ");
extern void Replace(std::string &result, const std::string& replaceWhat, const std::string& replaceWithWhat);

class Device
{
public:
    int ID;
    string Name;
    string PluginName;
    string InitialState;
	vector<vector<string>> m_overridePatches;
    void *m_node = NULL;
    void *m_gainNode = NULL;
    void *m_midiFilterNode = NULL;
    void* m_audioInputNode = NULL;
    bool m_usesBanks = false;
    bool m_hasStupidCrossfade = false;
    bool m_stereoToMonoWillPhase = false;
    bool m_doesntRespectSoundOff = false;
    bool m_deleted = false;
    float m_order = 0;

	template<class A>
	void Serialize(A& ar)
	{
		AR(ID, XmlAttribute);
        AR(Name, XmlAttribute);
        AR(PluginName, XmlAttribute);
        AR(InitialState);
    }
};

enum NoteMode
{
	Normal,
	Scoop,
	Fall,
	Sixteenth,
	DoubleOctave,
	ThreeOctaveArpeggio,
    NoSustain
};

class Zone
{
public:
    int DeviceID;
    Device *Device;
    int Bank;
    int Program;
    float Volume;
    bool Solo;
    bool Mute;
    int NoteMode;
    int Transpose;
    int LowKey;
    int HighKey;
    string OverrideState;

	template<class A>
	void Serialize(A& ar)
	{
        if (ar.IsSaving())
            Solo = false;

		AR(DeviceID, XmlAttribute);
        if (!ar.IsSaving() || Device->m_usesBanks)
		    AR(Bank, XmlAttribute | XmlOptional);

        AR(Program, XmlAttribute);
		AR(Volume, XmlAttribute | XmlOptional);
        AR(Solo, XmlAttribute | XmlOptional);
        AR(Mute, XmlAttribute | XmlOptional);
        AR(NoteMode, XmlAttribute | XmlOptional);
		AR(Transpose, XmlAttribute | XmlOptional);
		AR(LowKey, XmlAttribute | XmlOptional);
        AR(HighKey, XmlAttribute | XmlOptional, 127);
        AR(OverrideState);
    }
};

class PerformanceType
{
public:
    int ID;
    string Name;
    float Tempo;
    vector<Zone> Zone;

    PerformanceType()
    {
        Tempo = 120; // Starting from empty can have this not set
    }

	template<class A>
	void Serialize(A& ar)
	{
		AR(ID, XmlAttribute);
		AR(Name, XmlAttribute);
		AR(Tempo, XmlAttribute | XmlOptional, 120);
        
        auto oldZone = Zone;
        if (ar.IsSaving()) 
        {
            Zone.clear();
            for (unsigned int i = 0; i < oldZone.size(); ++i)
                if (!oldZone[i].Mute && (!oldZone[i].Device || !oldZone[i].Device->m_deleted))
                    Zone.push_back(oldZone[i]);
        }
		AR(Zone);
        if (ar.IsSaving())
            Zone = oldZone;
	}
};

class Integer
{
public:
    Integer(int id = 0) : ID(id) {}

    int ID;

    template<class A>
    void Serialize(A& ar)
    {
        AR(ID, XmlAttribute);
    }
};

class Song
{
public:
    int ID;
    string Name;
    vector<Integer> Performance;
    vector<PerformanceType*> PerformancePtr;

	template<class A>
	void Serialize(A& ar)
	{
		AR(ID, XmlAttribute);
		AR(Name, XmlAttribute);
		AR(Performance);
	}
};

class SetList
{
public:
	int ID;
    string Name;
    vector<Song*> SongPtr;
    vector<Integer> Song;

	template<class A>
	void Serialize(A& ar)
	{
		AR(ID, XmlAttribute);
		AR(Name, XmlAttribute);
		AR(Song);
	}
};

class RacksType
{
public:
    vector<Device> Rack;
    template<class A>
    void Serialize(A& ar)
    {
        if (ar.IsSaving())
        {
            auto oldRack = Rack;
            Rack.clear();
            for (int i = 0; i < oldRack.size(); ++i)
                if (!oldRack[i].m_deleted)
                    Rack.push_back(oldRack[i]);
            sort(Rack.begin(), Rack.end(), [](Device& a, Device& b) { return a.m_order < b.m_order; });
            AR(Rack);
            Rack = oldRack;
        }
        else
        {
            AR(Rack);
            for (int i = 0; i < Rack.size(); ++i)
                Rack[i].m_order = i;
        }
    }
};

class SetListsType
{
public:
    vector<SetList> SetList;
    template<class A>
    void Serialize(A& ar)
    {
        AR(SetList);
    }
};

class SongsType
{
public:
    vector<Song> Song;
    template<class A>
    void Serialize(A& ar)
    {
        AR(Song);
    }
};

class PerformancesType
{
public:
    vector<PerformanceType> Performance;
    template<class A>
    void Serialize(A& ar)
    {
        AR(Performance);
    }
};

class PerformerFile
{
public:

    PerformerFile()
    {
        Racks.Rack.reserve(100);
    }
    SetListsType SetLists;
    RacksType Racks;
    SongsType Songs;
    PerformancesType Performances;
	int CurrentSetListID;

	template<class A>
	void Serialize(A& ar)
	{
		AR(CurrentSetListID, XmlAttribute);
		AR(SetLists);
		AR(Racks);
		AR(Songs);
		AR(Performances);
	}
};

class Performer
{
public:

    Performer()
    {
        TempPerformance.Zone.reserve(100);
    }

    int m_currentPerformanceIndex = 0;
    int m_currentSetlistIndex = 0;
	PerformanceType TempPerformance;

    int GetTotalPerformances();
    void GetPerformanceByIndex(PerformanceType*&, Song *&, int index);
	int GetIndexByPerformance(int songID, int performanceID);

    void Import(const char *file);
    void ResolveIDs();

    template<class A>
    void Serialize(A& ar)
    {
        AR(Root);
        if (!ar.IsSaving())
        {
            AR(TempPerformance); // fill this with default data
            TempPerformance.ID = (int)Uuid().hash();
            TempPerformance.Zone.resize(Root.Racks.Rack.size());
            for (int i = 0; i < TempPerformance.Zone.size(); ++i)
            {
                AR(TempPerformance.Zone[i]);
                TempPerformance.Zone[i].DeviceID = Root.Racks.Rack[i].ID;
                TempPerformance.Zone[i].Device = &Root.Racks.Rack[i];
            }
        }
    }

    PerformerFile Root;
};
