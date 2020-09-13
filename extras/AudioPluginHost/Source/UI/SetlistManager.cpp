/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.5

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
#include "../Performer.h"
//[/Headers]

#include "SetlistManager.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
SetlistManager::SetlistManager ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    m_selectedSongGroup.reset (new GroupComponent (String(),
                                                   TRANS("Performances in selected song")));
    addAndMakeVisible (m_selectedSongGroup.get());

    m_selectedSongGroup->setBounds (496, 72, 216, 568);

    m_allsongsGroup.reset (new GroupComponent (String(),
                                               TRANS("Songs")));
    addAndMakeVisible (m_allsongsGroup.get());

    m_allsongsGroup->setBounds (216, 72, 280, 568);

    m_setlistGroup.reset (new GroupComponent (String(),
                                              TRANS("Setlist")));
    addAndMakeVisible (m_setlistGroup.get());

    m_setlistGroup->setBounds (0, 0, 216, 640);

    m_newSetlist.reset (new TextButton (String()));
    addAndMakeVisible (m_newSetlist.get());
    m_newSetlist->setButtonText (TRANS("New"));
    m_newSetlist->addListener (this);

    m_newSetlist->setBounds (8, 24, 39, 24);

    m_deleteSetlist.reset (new TextButton (String()));
    addAndMakeVisible (m_deleteSetlist.get());
    m_deleteSetlist->setButtonText (TRANS("Delete"));
    m_deleteSetlist->addListener (this);

    m_deleteSetlist->setBounds (48, 24, 47, 24);

    m_cloneSetlist.reset (new TextButton (String()));
    addAndMakeVisible (m_cloneSetlist.get());
    m_cloneSetlist->setButtonText (TRANS("Clone"));
    m_cloneSetlist->addListener (this);

    m_cloneSetlist->setBounds (96, 24, 47, 24);

    m_renameSetlist.reset (new TextButton (String()));
    addAndMakeVisible (m_renameSetlist.get());
    m_renameSetlist->setButtonText (TRANS("Rename"));
    m_renameSetlist->addListener (this);

    m_renameSetlist->setBounds (144, 24, 63, 24);

    m_currentSetlist.reset (new ComboBox (String()));
    addAndMakeVisible (m_currentSetlist.get());
    m_currentSetlist->setEditableText (false);
    m_currentSetlist->setJustificationType (Justification::centredLeft);
    m_currentSetlist->setTextWhenNothingSelected (String());
    m_currentSetlist->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    m_currentSetlist->addListener (this);

    m_currentSetlist->setBounds (8, 56, 200, 24);

    m_setlist.reset (new ListBox());
    addAndMakeVisible (m_setlist.get());

    m_setlist->setBounds (8, 120, 200, 512);

    m_useSetlistSong.reset (new TextButton (String()));
    addAndMakeVisible (m_useSetlistSong.get());
    m_useSetlistSong->setButtonText (TRANS("Use"));
    m_useSetlistSong->addListener (this);

    m_useSetlistSong->setBounds (16, 88, 31, 24);

    m_removeSetlistSong.reset (new TextButton (String()));
    addAndMakeVisible (m_removeSetlistSong.get());
    m_removeSetlistSong->setButtonText (TRANS("Remove"));
    m_removeSetlistSong->addListener (this);

    m_removeSetlistSong->setBounds (128, 88, 63, 24);

    m_upSetlistSong.reset (new TextButton (String()));
    addAndMakeVisible (m_upSetlistSong.get());
    m_upSetlistSong->setButtonText (TRANS("Up"));
    m_upSetlistSong->addListener (this);

    m_upSetlistSong->setBounds (48, 88, 31, 24);

    m_downSetlistSong.reset (new TextButton (String()));
    addAndMakeVisible (m_downSetlistSong.get());
    m_downSetlistSong->setButtonText (TRANS("Down"));
    m_downSetlistSong->addListener (this);

    m_downSetlistSong->setBounds (80, 88, 47, 24);

    m_useSong.reset (new TextButton (String()));
    addAndMakeVisible (m_useSong.get());
    m_useSong->setButtonText (TRANS("Use"));
    m_useSong->addListener (this);

    m_useSong->setBounds (264, 88, 31, 24);

    m_newSong.reset (new TextButton (String()));
    addAndMakeVisible (m_newSong.get());
    m_newSong->setButtonText (TRANS("New"));
    m_newSong->addListener (this);

    m_newSong->setBounds (224, 88, 39, 24);

    m_deleteSong.reset (new TextButton (String()));
    addAndMakeVisible (m_deleteSong.get());
    m_deleteSong->setButtonText (TRANS("Delete"));
    m_deleteSong->addListener (this);

    m_deleteSong->setBounds (296, 88, 47, 24);

    m_cloneSong.reset (new TextButton (String()));
    addAndMakeVisible (m_cloneSong.get());
    m_cloneSong->setButtonText (TRANS("Clone"));
    m_cloneSong->addListener (this);

    m_cloneSong->setBounds (344, 88, 47, 24);

    m_renameSong.reset (new TextButton (String()));
    addAndMakeVisible (m_renameSong.get());
    m_renameSong->setButtonText (TRANS("Rename"));
    m_renameSong->addListener (this);

    m_renameSong->setBounds (392, 88, 63, 24);

    m_addSong.reset (new TextButton (String()));
    addAndMakeVisible (m_addSong.get());
    m_addSong->setButtonText (TRANS("<<"));
    m_addSong->addListener (this);

    m_addSong->setBounds (456, 88, 31, 24);

    m_songList.reset (new ListBox());
    addAndMakeVisible (m_songList.get());

    m_songList->setBounds (224, 120, 264, 512);

    m_performancesInSongList.reset (new ListBox());
    addAndMakeVisible (m_performancesInSongList.get());

    m_performancesInSongList->setBounds (504, 120, 200, 512);

    m_usePerformanceInsSong.reset (new TextButton (String()));
    addAndMakeVisible (m_usePerformanceInsSong.get());
    m_usePerformanceInsSong->setButtonText (TRANS("Use"));
    m_usePerformanceInsSong->addListener (this);

    m_usePerformanceInsSong->setBounds (512, 88, 31, 24);

    m_removePerformanceFromSong.reset (new TextButton (String()));
    addAndMakeVisible (m_removePerformanceFromSong.get());
    m_removePerformanceFromSong->setButtonText (TRANS("Remove"));
    m_removePerformanceFromSong->addListener (this);

    m_removePerformanceFromSong->setBounds (624, 88, 63, 24);

    m_upPerformanceInSong.reset (new TextButton (String()));
    addAndMakeVisible (m_upPerformanceInSong.get());
    m_upPerformanceInSong->setButtonText (TRANS("Up"));
    m_upPerformanceInSong->addListener (this);

    m_upPerformanceInSong->setBounds (544, 88, 31, 24);

    m_downPerformanceInSong.reset (new TextButton (String()));
    addAndMakeVisible (m_downPerformanceInSong.get());
    m_downPerformanceInSong->setButtonText (TRANS("Down"));
    m_downPerformanceInSong->addListener (this);

    m_downPerformanceInSong->setBounds (576, 88, 47, 24);

    m_performancesGroup.reset (new GroupComponent (String(),
                                                   TRANS("Performances")));
    addAndMakeVisible (m_performancesGroup.get());

    m_performancesGroup->setBounds (712, 72, 232, 568);

    m_usePerformance.reset (new TextButton (String()));
    addAndMakeVisible (m_usePerformance.get());
    m_usePerformance->setButtonText (TRANS("Use"));
    m_usePerformance->addListener (this);

    m_usePerformance->setBounds (760, 88, 31, 24);

    m_newPerformance.reset (new TextButton (String()));
    addAndMakeVisible (m_newPerformance.get());
    m_newPerformance->setButtonText (TRANS("New"));
    m_newPerformance->addListener (this);

    m_newPerformance->setBounds (720, 88, 39, 24);

    m_deletePerformance.reset (new TextButton (String()));
    addAndMakeVisible (m_deletePerformance.get());
    m_deletePerformance->setButtonText (TRANS("Delete"));
    m_deletePerformance->addListener (this);

    m_deletePerformance->setBounds (792, 88, 47, 24);

    m_renamePerformance.reset (new TextButton (String()));
    addAndMakeVisible (m_renamePerformance.get());
    m_renamePerformance->setButtonText (TRANS("Rename"));
    m_renamePerformance->addListener (this);

    m_renamePerformance->setBounds (840, 88, 63, 24);

    m_addPerformance.reset (new TextButton (String()));
    addAndMakeVisible (m_addPerformance.get());
    m_addPerformance->setButtonText (TRANS("<<"));
    m_addPerformance->addListener (this);

    m_addPerformance->setBounds (904, 88, 31, 24);

    m_performanceList.reset (new ListBox());
    addAndMakeVisible (m_performanceList.get());

    m_performanceList->setBounds (720, 120, 216, 512);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (944, 640);


    //[Constructor] You can add your own custom stuff here..
	m_performanceListModel = new PerformanceListModel();
	m_performanceList->setModel(m_performanceListModel);

	m_selectedSongListModel = new SelectedSongListModel();
	m_selectedSongListModel->m_onSelectedChanged = [this]()
	{
		if (m_performancesInSongList->getSelectedRow() != -1)
		{
			for (int i = 0; i < m_performer->Root.Performances.Performance.size(); ++i)
				if (m_performer->Root.Performances.Performance[i].ID == m_selectedSongListModel->m_selectedSong->PerformancePtr[m_performancesInSongList->getSelectedRow()]->ID)
					m_performanceList->selectRow(i);
		}
	};
	m_performancesInSongList->setModel(m_selectedSongListModel);

	m_setlistListModel = new SetlistListModel();
	m_setlistListModel->m_onSelectedChanged = [this]()
	{
		if (m_setlist->getSelectedRow() != -1)
		{
			for (int i = 0; i < m_performer->Root.Songs.Song.size(); ++i)
				if (m_performer->Root.Songs.Song[i].ID == m_setlistListModel->m_selectedSetlist->SongPtr[m_setlist->getSelectedRow()]->ID)
					m_songList->selectRow(i);
		}
	};
	m_setlist->setModel(m_setlistListModel);

	m_songListModel = new SongListModel();
	m_songListModel->m_onSelectedChanged = [this]()
	{
		if (m_songList->getSelectedRow() != -1)
		{
			m_selectedSongListModel->m_selectedSong = &m_performer->Root.Songs.Song[m_songList->getSelectedRow()];
			m_performancesInSongList->updateContent();
			m_performancesInSongList->repaint();
		}
	};
	m_songList->setModel(m_songListModel);
    //[/Constructor]
}

SetlistManager::~SetlistManager()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    m_selectedSongGroup = nullptr;
    m_allsongsGroup = nullptr;
    m_setlistGroup = nullptr;
    m_newSetlist = nullptr;
    m_deleteSetlist = nullptr;
    m_cloneSetlist = nullptr;
    m_renameSetlist = nullptr;
    m_currentSetlist = nullptr;
    m_setlist = nullptr;
    m_useSetlistSong = nullptr;
    m_removeSetlistSong = nullptr;
    m_upSetlistSong = nullptr;
    m_downSetlistSong = nullptr;
    m_useSong = nullptr;
    m_newSong = nullptr;
    m_deleteSong = nullptr;
    m_cloneSong = nullptr;
    m_renameSong = nullptr;
    m_addSong = nullptr;
    m_songList = nullptr;
    m_performancesInSongList = nullptr;
    m_usePerformanceInsSong = nullptr;
    m_removePerformanceFromSong = nullptr;
    m_upPerformanceInSong = nullptr;
    m_downPerformanceInSong = nullptr;
    m_performancesGroup = nullptr;
    m_usePerformance = nullptr;
    m_newPerformance = nullptr;
    m_deletePerformance = nullptr;
    m_renamePerformance = nullptr;
    m_addPerformance = nullptr;
    m_performanceList = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void SetlistManager::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void SetlistManager::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void SetlistManager::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == m_newSetlist.get())
    {
        //[UserButtonCode_m_newSetlist] -- add your button handler code here..
        //[/UserButtonCode_m_newSetlist]
    }
    else if (buttonThatWasClicked == m_deleteSetlist.get())
    {
        //[UserButtonCode_m_deleteSetlist] -- add your button handler code here..
		int index = m_currentSetlist->getSelectedItemIndex();
		if (index != -1)
		{
			m_performer->Root.SetLists.SetList.erase(m_performer->Root.SetLists.SetList.begin() + index);

			m_performer->m_currentSetlistIndex = 0;
			if (m_performer->Root.SetLists.SetList.size())
			{
				m_setlistListModel->m_selectedSetlist = &m_performer->Root.SetLists.SetList[0];
				m_performer->Root.CurrentSetListID = m_performer->Root.SetLists.SetList[0].ID;
			}
			else
			{
				m_setlistListModel->m_selectedSetlist = nullptr;
				m_performer->Root.CurrentSetListID = 0;
			}

			UpdateSelectedSetlist();

			m_setlist->updateContent();
			m_setlist->repaint();
		}
        //[/UserButtonCode_m_deleteSetlist]
    }
    else if (buttonThatWasClicked == m_cloneSetlist.get())
    {
        //[UserButtonCode_m_cloneSetlist] -- add your button handler code here..
        //[/UserButtonCode_m_cloneSetlist]
    }
    else if (buttonThatWasClicked == m_renameSetlist.get())
    {
        //[UserButtonCode_m_renameSetlist] -- add your button handler code here..
		Rename(m_setlistListModel->m_selectedSetlist->Name);
		UpdateSelectedSetlist();
        //[/UserButtonCode_m_renameSetlist]
    }
    else if (buttonThatWasClicked == m_useSetlistSong.get())
    {
        //[UserButtonCode_m_useSetlistSong] -- add your button handler code here..
        //[/UserButtonCode_m_useSetlistSong]
    }
    else if (buttonThatWasClicked == m_removeSetlistSong.get())
    {
        //[UserButtonCode_m_removeSetlistSong] -- add your button handler code here..
        //[/UserButtonCode_m_removeSetlistSong]
    }
    else if (buttonThatWasClicked == m_upSetlistSong.get())
    {
        //[UserButtonCode_m_upSetlistSong] -- add your button handler code here..
		auto index = m_setlist->getSelectedRow();
		if (index > 0)
		{
			swap(m_setlistListModel->m_selectedSetlist->Song[index], m_setlistListModel->m_selectedSetlist->Song[index - 1]);
			swap(m_setlistListModel->m_selectedSetlist->SongPtr[index], m_setlistListModel->m_selectedSetlist->SongPtr[index - 1]);
			m_setlist->selectRow(index - 1);
			m_setlist->updateContent();
			m_setlist->repaint();
		}
        //[/UserButtonCode_m_upSetlistSong]
    }
    else if (buttonThatWasClicked == m_downSetlistSong.get())
    {
        //[UserButtonCode_m_downSetlistSong] -- add your button handler code here..
		auto index = m_setlist->getSelectedRow();
		if (index != -1 && index < m_setlistListModel->m_selectedSetlist->Song.size() - 1)
		{
			swap(m_setlistListModel->m_selectedSetlist->Song[index], m_setlistListModel->m_selectedSetlist->Song[index + 1]);
			swap(m_setlistListModel->m_selectedSetlist->SongPtr[index], m_setlistListModel->m_selectedSetlist->SongPtr[index + 1]);
			m_setlist->selectRow(index + 1);
			m_setlist->updateContent();
			m_setlist->repaint();
		}
        //[/UserButtonCode_m_downSetlistSong]
    }
    else if (buttonThatWasClicked == m_useSong.get())
    {
        //[UserButtonCode_m_useSong] -- add your button handler code here..
		// find song in setlist, if not there then deselect
		if (m_songList->getSelectedRow() != -1)
		{
			bool found = false;
			for (int i = 0; i < m_setlistListModel->m_selectedSetlist->Song.size(); ++i)
			{
				if (m_setlistListModel->m_selectedSetlist->Song[i].ID == m_performer->Root.Songs.Song[m_songList->getSelectedRow()].ID)
				{
					found = true;
					m_setlist->selectRow(i);
				}
			}
			if (!found)
				m_setlist->deselectAllRows();
		}
        //[/UserButtonCode_m_useSong]
    }
    else if (buttonThatWasClicked == m_newSong.get())
    {
        //[UserButtonCode_m_newSong] -- add your button handler code here..
        //[/UserButtonCode_m_newSong]
    }
    else if (buttonThatWasClicked == m_deleteSong.get())
    {
        //[UserButtonCode_m_deleteSong] -- add your button handler code here..
		int index = m_songList->getSelectedRow();
		if (index != -1)
		{
			// remove from setlists
			for (auto sl = 0U; sl < m_performer->Root.SetLists.SetList.size(); ++sl)
				for (auto sg = 0U; sg < m_performer->Root.SetLists.SetList[sl].Song.size(); ++sg)
					if (m_performer->Root.SetLists.SetList[sl].Song[sg].ID == m_performer->Root.Songs.Song[index].ID)
					{
						m_performer->Root.SetLists.SetList[sl].Song.erase(m_performer->Root.SetLists.SetList[sl].Song.begin() + sg);
						m_performer->Root.SetLists.SetList[sl].SongPtr.erase(m_performer->Root.SetLists.SetList[sl].SongPtr.begin() + sg);
					}

			m_performer->Root.Songs.Song.erase(m_performer->Root.Songs.Song.begin()+index);

			UpdatePointers();

			m_selectedSongListModel->m_selectedSong = nullptr;
			m_performancesInSongList->updateContent();
			m_performancesInSongList->repaint();
			m_setlist->updateContent();
			m_setlist->repaint();
			m_songList->updateContent();
			m_songList->repaint();
		}
        //[/UserButtonCode_m_deleteSong]
    }
    else if (buttonThatWasClicked == m_cloneSong.get())
    {
        //[UserButtonCode_m_cloneSong] -- add your button handler code here..
        //[/UserButtonCode_m_cloneSong]
    }
    else if (buttonThatWasClicked == m_renameSong.get())
    {
        //[UserButtonCode_m_renameSong] -- add your button handler code here..
		if (m_songList->getSelectedRow() != -1)
		{
			Rename(m_performer->Root.Songs.Song[m_songList->getSelectedRow()].Name);
			SortSongs();
			UpdatePointers();
			m_songList->deselectAllRows();
			m_songList->updateContent();
			m_songList->repaint();
		}
        //[/UserButtonCode_m_renameSong]
    }
    else if (buttonThatWasClicked == m_addSong.get())
    {
        //[UserButtonCode_m_addSong] -- add your button handler code here..
        //[/UserButtonCode_m_addSong]
    }
    else if (buttonThatWasClicked == m_usePerformanceInsSong.get())
    {
        //[UserButtonCode_m_usePerformanceInsSong] -- add your button handler code here..
        //[/UserButtonCode_m_usePerformanceInsSong]
    }
    else if (buttonThatWasClicked == m_removePerformanceFromSong.get())
    {
        //[UserButtonCode_m_removePerformanceFromSong] -- add your button handler code here..
        //[/UserButtonCode_m_removePerformanceFromSong]
    }
    else if (buttonThatWasClicked == m_upPerformanceInSong.get())
    {
        //[UserButtonCode_m_upPerformanceInSong] -- add your button handler code here..
        //[/UserButtonCode_m_upPerformanceInSong]
    }
    else if (buttonThatWasClicked == m_downPerformanceInSong.get())
    {
        //[UserButtonCode_m_downPerformanceInSong] -- add your button handler code here..
        //[/UserButtonCode_m_downPerformanceInSong]
    }
    else if (buttonThatWasClicked == m_usePerformance.get())
    {
        //[UserButtonCode_m_usePerformance] -- add your button handler code here..
		// Find first song in setlist that uses performance
		if (m_performanceList->getSelectedRow() != -1)
		{
			bool found = false;
			for (int i = 0; i < m_setlistListModel->m_selectedSetlist->SongPtr.size(); ++i)
			{
				for (int j = 0; j < m_setlistListModel->m_selectedSetlist->SongPtr[i]->Performance.size(); ++j)
				{
					if (m_setlistListModel->m_selectedSetlist->SongPtr[i]->Performance[j].ID == m_performer->Root.Performances.Performance[m_performanceList->getSelectedRow()].ID)
					{
						found = true;
						m_setlist->selectRow(i);
						m_performancesInSongList->selectRow(j);
						i = (int)m_setlistListModel->m_selectedSetlist->SongPtr.size();
						break;
					}
				}
			}
			if (!found)
			{
				m_selectedSongListModel->m_selectedSong = nullptr;
				m_performancesInSongList->updateContent();
				m_performancesInSongList->repaint();

				m_songList->deselectAllRows();
				m_setlist->deselectAllRows();
			}
		}
        //[/UserButtonCode_m_usePerformance]
    }
    else if (buttonThatWasClicked == m_newPerformance.get())
    {
        //[UserButtonCode_m_newPerformance] -- add your button handler code here..
        //[/UserButtonCode_m_newPerformance]
    }
    else if (buttonThatWasClicked == m_deletePerformance.get())
    {
        //[UserButtonCode_m_deletePerformance] -- add your button handler code here..
		int index = m_performanceList->getSelectedRow();
		if (index != -1)
		{
			// remove from songs
			for (auto sg = 0U; sg < m_performer->Root.Songs.Song.size(); ++sg)
				for (auto p = 0U; p < m_performer->Root.Songs.Song[sg].Performance.size(); ++p)
					if (m_performer->Root.Songs.Song[sg].Performance[p].ID == m_performer->Root.Performances.Performance[index].ID)
					{
						m_performer->Root.Songs.Song[sg].Performance.erase(m_performer->Root.Songs.Song[sg].Performance.begin() + p);
						m_performer->Root.Songs.Song[sg].PerformancePtr.erase(m_performer->Root.Songs.Song[sg].PerformancePtr.begin() + p);
					}

			m_performer->Root.Performances.Performance.erase(m_performer->Root.Performances.Performance.begin() + index);

			UpdatePointers();

			m_performancesInSongList->updateContent();
			m_performancesInSongList->repaint();
			m_performanceList->updateContent();
			m_performanceList->repaint();
		}
        //[/UserButtonCode_m_deletePerformance]
    }
    else if (buttonThatWasClicked == m_renamePerformance.get())
    {
        //[UserButtonCode_m_renamePerformance] -- add your button handler code here..
		if (m_performanceList->getSelectedRow() != -1)
		{
			Rename(m_performer->Root.Performances.Performance[m_performanceList->getSelectedRow()].Name);
			SortPerformances();
			UpdatePointers();
			m_performanceList->deselectAllRows();
			m_performanceList->updateContent();
			m_performanceList->repaint();
		}
        //[/UserButtonCode_m_renamePerformance]
    }
    else if (buttonThatWasClicked == m_addPerformance.get())
    {
        //[UserButtonCode_m_addPerformance] -- add your button handler code here..
        //[/UserButtonCode_m_addPerformance]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void SetlistManager::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == m_currentSetlist.get())
    {
        //[UserComboBoxCode_m_currentSetlist] -- add your combo box handling code here..
		if (comboBoxThatHasChanged->getSelectedItemIndex() != -1)
		{
			m_performer->m_currentSetlistIndex = comboBoxThatHasChanged->getSelectedItemIndex();
			m_setlistListModel->m_selectedSetlist = &m_performer->Root.SetLists.SetList[m_performer->m_currentSetlistIndex];
			m_performer->Root.CurrentSetListID = comboBoxThatHasChanged->getSelectedId();
			m_setlist->updateContent();
			m_setlist->repaint();
		}
        //[/UserComboBoxCode_m_currentSetlist]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void SetlistManager::SortSongs()
{
	auto &list = m_performer->Root.Songs.Song;
	for (int i = 0; i < (int)list.size() - 1; ++i)
	{
		for (int j = 0; j < (int)list.size() - i - 1; ++j)
		{
			if (list[j].Name > list[j + 1].Name)
			{
				swap(list[j], list[j + 1]);
			}
		}
	}
}
void SetlistManager::SortPerformances()
{
	auto &list = m_performer->Root.Performances.Performance;
	for (int i = 0; i < (int)list.size() - 1; ++i)
	{
		for (int j = 0; j < (int)list.size() - i - 1; ++j)
		{
			if (list[j].Name > list[j + 1].Name)
			{
				swap(list[j], list[j + 1]);
			}
		}
	}
}

void SetlistManager::Rename(std::string &str)
{
	AlertWindow alert("", "", AlertWindow::NoIcon);
	alert.addTextEditor("Name", str);
	alert.addButton("Cancel", 0);
	alert.addButton("Ok", 1);
	auto ret = alert.runModalLoop();
	if (ret == 1)
		str = alert.getTextEditorContents("Name").toStdString();
}

void SetlistManager::UpdateSelectedSetlist()
{
	m_currentSetlist->clear();
	for (int i = 0; i < m_performer->Root.SetLists.SetList.size(); ++i)
		m_currentSetlist->addItem(m_performer->Root.SetLists.SetList[i].Name, m_performer->Root.SetLists.SetList[i].ID);
	m_currentSetlist->setSelectedId(m_performer->Root.CurrentSetListID);
}


void SetlistManager::UpdatePointers()
{
	// Update pointers
	for (auto sl = 0U; sl < m_performer->Root.SetLists.SetList.size(); ++sl)
		for (auto sg = 0U; sg < m_performer->Root.SetLists.SetList[sl].Song.size(); ++sg)
			for (auto i = 0U; i < m_performer->Root.Songs.Song.size(); ++i)
				if (m_performer->Root.SetLists.SetList[sl].Song[sg].ID == m_performer->Root.Songs.Song[i].ID)
					m_performer->Root.SetLists.SetList[sl].SongPtr[sg] = &m_performer->Root.Songs.Song[i];

	for (auto sg = 0U; sg < m_performer->Root.Songs.Song.size(); ++sg)
		for (auto p = 0U; p < m_performer->Root.Songs.Song[sg].Performance.size(); ++p)
			for (auto i = 0U; i < m_performer->Root.Performances.Performance.size(); ++i)
				if (m_performer->Root.Songs.Song[sg].Performance[p].ID == m_performer->Root.Performances.Performance[i].ID)
					m_performer->Root.Songs.Song[sg].PerformancePtr[p] = &m_performer->Root.Performances.Performance[i];
}

void SetlistManager::SetData(Performer *performer)
{
	m_performer = performer;

	SortSongs();
	SortPerformances();
	UpdatePointers();

	if (m_performer->m_currentSetlistIndex < m_performer->Root.SetLists.SetList.size())
		m_setlistListModel->m_selectedSetlist = &m_performer->Root.SetLists.SetList[m_performer->m_currentSetlistIndex];

	m_performanceListModel->m_performer = m_performer;
	m_songListModel->m_performer = m_performer;

	m_performanceList->updateContent();
	m_performanceList->repaint();
	m_songList->updateContent();
	m_songList->repaint();
	m_performancesInSongList->updateContent();
	m_performancesInSongList->repaint();
	m_setlist->updateContent();
	m_setlist->repaint();

	UpdateSelectedSetlist();
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SetlistManager" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="944" initialHeight="640">
  <BACKGROUND backgroundColour="ff323e44"/>
  <GROUPCOMPONENT name="" id="f1d8b2e4a6a553be" memberName="m_selectedSongGroup"
                  virtualName="" explicitFocusOrder="0" pos="496 72 216 568" title="Performances in selected song"/>
  <GROUPCOMPONENT name="" id="f83e67c89c18873a" memberName="m_allsongsGroup" virtualName=""
                  explicitFocusOrder="0" pos="216 72 280 568" title="Songs"/>
  <GROUPCOMPONENT name="" id="6da09d848d52cf31" memberName="m_setlistGroup" virtualName=""
                  explicitFocusOrder="0" pos="0 0 216 640" title="Setlist"/>
  <TEXTBUTTON name="" id="56d8b45afd26967b" memberName="m_newSetlist" virtualName=""
              explicitFocusOrder="0" pos="8 24 39 24" buttonText="New" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="ef98fa29a47de712" memberName="m_deleteSetlist" virtualName=""
              explicitFocusOrder="0" pos="48 24 47 24" buttonText="Delete"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="5c9c5c0c8f2163b1" memberName="m_cloneSetlist" virtualName=""
              explicitFocusOrder="0" pos="96 24 47 24" buttonText="Clone" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="489d309249c38b97" memberName="m_renameSetlist" virtualName=""
              explicitFocusOrder="0" pos="144 24 63 24" buttonText="Rename"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <COMBOBOX name="" id="5509043ea4b4bc0b" memberName="m_currentSetlist" virtualName=""
            explicitFocusOrder="0" pos="8 56 200 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <GENERICCOMPONENT name="" id="8123f414b672ade2" memberName="m_setlist" virtualName=""
                    explicitFocusOrder="0" pos="8 120 200 512" class="ListBox" params=""/>
  <TEXTBUTTON name="" id="84d0f16ec84ab858" memberName="m_useSetlistSong" virtualName=""
              explicitFocusOrder="0" pos="16 88 31 24" buttonText="Use" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="fe8902859c1b31cc" memberName="m_removeSetlistSong"
              virtualName="" explicitFocusOrder="0" pos="128 88 63 24" buttonText="Remove"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="8868b74e046a687e" memberName="m_upSetlistSong" virtualName=""
              explicitFocusOrder="0" pos="48 88 31 24" buttonText="Up" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="b698c43479310e1b" memberName="m_downSetlistSong"
              virtualName="" explicitFocusOrder="0" pos="80 88 47 24" buttonText="Down"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="71d422f359c90f09" memberName="m_useSong" virtualName=""
              explicitFocusOrder="0" pos="264 88 31 24" buttonText="Use" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="88bec898dc9f83ce" memberName="m_newSong" virtualName=""
              explicitFocusOrder="0" pos="224 88 39 24" buttonText="New" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="94624099b31d527d" memberName="m_deleteSong" virtualName=""
              explicitFocusOrder="0" pos="296 88 47 24" buttonText="Delete"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="44d3fd96a1be472d" memberName="m_cloneSong" virtualName=""
              explicitFocusOrder="0" pos="344 88 47 24" buttonText="Clone"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="1d55b5a57cb88daa" memberName="m_renameSong" virtualName=""
              explicitFocusOrder="0" pos="392 88 63 24" buttonText="Rename"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="99bdddc76e79dad4" memberName="m_addSong" virtualName=""
              explicitFocusOrder="0" pos="456 88 31 24" buttonText="&lt;&lt;"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <GENERICCOMPONENT name="" id="16b3d817e9bbdfa3" memberName="m_songList" virtualName=""
                    explicitFocusOrder="0" pos="224 120 264 512" class="ListBox"
                    params=""/>
  <GENERICCOMPONENT name="" id="dae16b07e383ee18" memberName="m_performancesInSongList"
                    virtualName="" explicitFocusOrder="0" pos="504 120 200 512" class="ListBox"
                    params=""/>
  <TEXTBUTTON name="" id="5473dab1a52c38b8" memberName="m_usePerformanceInsSong"
              virtualName="" explicitFocusOrder="0" pos="512 88 31 24" buttonText="Use"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="1e1ee19c0bcd2e45" memberName="m_removePerformanceFromSong"
              virtualName="" explicitFocusOrder="0" pos="624 88 63 24" buttonText="Remove"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="aadabee9abf8bfea" memberName="m_upPerformanceInSong"
              virtualName="" explicitFocusOrder="0" pos="544 88 31 24" buttonText="Up"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="8b6c6b593bc07036" memberName="m_downPerformanceInSong"
              virtualName="" explicitFocusOrder="0" pos="576 88 47 24" buttonText="Down"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <GROUPCOMPONENT name="" id="efd2f83ca8a44509" memberName="m_performancesGroup"
                  virtualName="" explicitFocusOrder="0" pos="712 72 232 568" title="Performances"/>
  <TEXTBUTTON name="" id="79cb7837b6ffb5d2" memberName="m_usePerformance" virtualName=""
              explicitFocusOrder="0" pos="760 88 31 24" buttonText="Use" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="7e4db04fec0cd4da" memberName="m_newPerformance" virtualName=""
              explicitFocusOrder="0" pos="720 88 39 24" buttonText="New" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="46cee244a77b1151" memberName="m_deletePerformance"
              virtualName="" explicitFocusOrder="0" pos="792 88 47 24" buttonText="Delete"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="2602716c741af53a" memberName="m_renamePerformance"
              virtualName="" explicitFocusOrder="0" pos="840 88 63 24" buttonText="Rename"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="df09317d45f8e745" memberName="m_addPerformance" virtualName=""
              explicitFocusOrder="0" pos="904 88 31 24" buttonText="&lt;&lt;"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <GENERICCOMPONENT name="" id="edd6eedf91731030" memberName="m_performanceList"
                    virtualName="" explicitFocusOrder="0" pos="720 120 216 512" class="ListBox"
                    params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

