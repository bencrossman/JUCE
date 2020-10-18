#pragma once

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
    void *m_node = NULL;
    void *m_gainNode = NULL;
    void *m_midiFilterNode = NULL;
    bool m_usesBanks = false;
	bool m_stereoToMonoWillPhase = false;

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
	ThreeOctaveArpeggio
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

	template<class A>
	void Serialize(A& ar)
	{
		AR(ID, XmlAttribute);
		AR(Name, XmlAttribute);
		AR(Tempo, XmlAttribute);
        
        auto oldZone = Zone;
        if (ar.IsSaving()) 
        {
            for (unsigned int i = 0; i < Zone.size(); ++i)
            {
                if (Zone[i].Mute)
                {
                    swap(Zone[i], Zone.back());
                    Zone.pop_back();
                    i--;
                }
            }
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
        AR(Rack);
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

    int m_currentPerformanceIndex = 0;
    int m_currentSetlistIndex = 0;
	PerformanceType TempPerformance;

    int GetTotalPerformances();
    void GetPerformanceByIndex(PerformanceType*&, Song *&, int index);

    void Import(const char *file);
    void ResolveIDs();

    template<class A>
    void Serialize(A& ar)
    {
        AR(Root);
    }

    PerformerFile Root;
};