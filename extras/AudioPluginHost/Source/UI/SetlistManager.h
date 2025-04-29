/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 7.0.10

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../../JuceLibraryCode/JuceHeader.h"

class SelectedSongListModel : public ListBoxModel
{
public:

	int getNumRows() override
	{
		if (m_selectedSong == nullptr)
			return 0;

		auto list = m_selectedSong->PerformancePtr;
		return (int)list.size();
	}

	void selectedRowsChanged(int) override
	{
		m_onSelectedChanged();
	}

	void paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override
	{
		auto list = m_selectedSong->PerformancePtr;
		if (rowIsSelected)
			g.fillAll(Colours::grey);
		g.setColour(Colours::white);
			if (rowNumber < list.size())
		g.drawText(list[rowNumber]->Name, 2, 0, width, height, Justification::centredLeft, true);
	}

	std::function<void()> m_onSelectedChanged;
	Song *m_selectedSong = nullptr;
};



class PerformanceListModel : public ListBoxModel
{
public:

	int getNumRows() override
	{
		if (m_performer == nullptr)
			return 0;

		auto list = m_performer->Root.Performances.Performance;
		return (int)list.size();
	}

	void paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override
	{
		auto list = m_performer->Root.Performances.Performance;

		if (rowNumber >= list.size())
			return;

		if (rowIsSelected)
			g.fillAll(Colours::grey);
		g.setColour(Colours::white);
		g.drawText(list[rowNumber].Name, 2, 0, width, height, Justification::centredLeft, true);
	}

	Performer *m_performer = nullptr;
};

class SongListModel : public ListBoxModel
{
public:

	int getNumRows() override
	{
		if (m_performer == nullptr)
			return 0;

		auto list = m_performer->Root.Songs.Song;
		return (int)list.size();
	}

	void selectedRowsChanged(int) override
	{
		m_onSelectedChanged();
	}

	void paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override
	{
		auto list = m_performer->Root.Songs.Song;

		if (rowNumber >= list.size())
			return;

		if (rowIsSelected)
			g.fillAll(Colours::grey);
		g.setColour(Colours::white);
		g.drawText(list[rowNumber].Name, 2, 0, width, height, Justification::centredLeft, true);
	}

	Performer *m_performer = nullptr;
	std::function<void()> m_onSelectedChanged;
};

class SetlistListModel : public ListBoxModel
{
public:

	int getNumRows() override
	{
		if (m_selectedSetlist == nullptr)
			return 0;

		auto list = m_selectedSetlist->SongPtr;
		return (int)list.size();
	}

	void selectedRowsChanged(int) override
	{
		m_onSelectedChanged();
	}

	void paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override
	{
		auto list = m_selectedSetlist->SongPtr;

		if (rowIsSelected)
			g.fillAll(Colours::grey);
		g.setColour(Colours::white);
		if (rowNumber < list.size())
			g.drawText(list[rowNumber]->Name, 2, 0, width, height, Justification::centredLeft, true);
	}

	SetList *m_selectedSetlist = nullptr;
	std::function<void()> m_onSelectedChanged;
};

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class SetlistManager  : public Component,
                        public juce::Button::Listener,
                        public juce::ComboBox::Listener
{
public:
    //==============================================================================
    SetlistManager ();
    ~SetlistManager() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void SetData(Performer *performer);
	Performer *m_performer = nullptr;
	std::function<void(int,PerformanceType *)> m_onUsePerformance;
	virtual void visibilityChanged();
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	PerformanceListModel *m_performanceListModel = nullptr;
	SongListModel *m_songListModel = nullptr;
	SelectedSongListModel *m_selectedSongListModel = nullptr;
	SetlistListModel *m_setlistListModel = nullptr;
	void UpdatePointers();
	void UpdateSelectedSetlist();
	void Rename(std::string &str);
	void SortSongs();
	void SortPerformances();
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::GroupComponent> m_selectedSongGroup;
    std::unique_ptr<juce::GroupComponent> m_allsongsGroup;
    std::unique_ptr<juce::GroupComponent> m_setlistGroup;
    std::unique_ptr<juce::TextButton> m_newSetlist;
    std::unique_ptr<juce::TextButton> m_deleteSetlist;
    std::unique_ptr<juce::TextButton> m_cloneSetlist;
    std::unique_ptr<juce::TextButton> m_renameSetlist;
    std::unique_ptr<juce::ComboBox> m_currentSetlist;
    std::unique_ptr<ListBox> m_setlist;
    std::unique_ptr<juce::TextButton> m_useSetlistSong;
    std::unique_ptr<juce::TextButton> m_removeSetlistSong;
    std::unique_ptr<juce::TextButton> m_upSetlistSong;
    std::unique_ptr<juce::TextButton> m_downSetlistSong;
    std::unique_ptr<juce::TextButton> m_useSong;
    std::unique_ptr<juce::TextButton> m_newSong;
    std::unique_ptr<juce::TextButton> m_deleteSong;
    std::unique_ptr<juce::TextButton> m_cloneSong;
    std::unique_ptr<juce::TextButton> m_renameSong;
    std::unique_ptr<juce::TextButton> m_addSong;
    std::unique_ptr<ListBox> m_songList;
    std::unique_ptr<ListBox> m_performancesInSongList;
    std::unique_ptr<juce::TextButton> m_usePerformanceInsSong;
    std::unique_ptr<juce::TextButton> m_removePerformanceFromSong;
    std::unique_ptr<juce::TextButton> m_upPerformanceInSong;
    std::unique_ptr<juce::TextButton> m_downPerformanceInSong;
    std::unique_ptr<juce::GroupComponent> m_performancesGroup;
    std::unique_ptr<juce::TextButton> m_usePerformance;
    std::unique_ptr<juce::TextButton> m_newPerformance;
    std::unique_ptr<juce::TextButton> m_deletePerformance;
    std::unique_ptr<juce::TextButton> m_renamePerformance;
    std::unique_ptr<juce::TextButton> m_addPerformance;
    std::unique_ptr<ListBox> m_performanceList;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SetlistManager)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

