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

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


class TableDemoComponent    : public Component, public TableListBoxModel
{
public:
    TableDemoComponent()
    {
        m_patches=NULL;
        // Create our table component and add it to this component..
        addAndMakeVisible (table);
        table.setModel (this);

        table.getHeader().addColumn ("PC",1,30);
        table.getHeader().addColumn ("Wav file",2,330);
        table.getHeader().addColumn ("Loop",3,40);

        //table.setMultipleSelectionEnabled (true);
    }

    // This is overloaded from TableListBoxModel, and must return the total number of rows in our table
    int getNumRows() override
    {
        return m_patches?(*m_patches).size():0;
    }

    void SetList(std::vector<Patch> *patches) { m_patches=patches; }

    // This is overloaded from TableListBoxModel, and should fill in the background of the whole row
    void paintRowBackground (Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) override
    {
        if (rowIsSelected)
            g.fillAll (Colours::lightblue);
        else if (rowNumber % 2)
            g.fillAll (Colour (0xffeeeeee));
    }

    // This is overloaded from TableListBoxModel, and must paint any cells that aren't using custom
    // components.
    void paintCell (Graphics& g, int rowNumber, int columnId,
                    int width, int height, bool /*rowIsSelected*/) override
    {
      String text;
      if (columnId==1)
        text=String(rowNumber+1);
      else if (columnId==2)
        text=(*m_patches)[rowNumber].m_file.c_str();
      else if (columnId==3)
        text=(*m_patches)[rowNumber].m_loop?"yes":"no";
      g.drawText (text, 2, 0, width - 4, height, Justification::centredLeft, true);

    }

    void resized() override
    {
        // position our table with a gap around its edge
        table.setBoundsInset (BorderSize<int> (8));
    }

    void AddItem(const char*newItem)
    {
      Patch newPatch;
      newPatch.m_file=newItem;
      newPatch.m_loop=false;
      (*m_patches).push_back(newPatch);
      table.updateContent();
    }

    void DeleteSelected()
    {
      int i=table.getSelectedRow();
      if (i!=-1)
      {
        (*m_patches).erase((*m_patches).begin() + i);
        table.updateContent();
      }
    }

    void MoveUpSelected()
    {
      int i=table.getSelectedRow();
      if (i>0)
      {
        std::swap((*m_patches)[i],(*m_patches)[i-1]);
        SparseSet<int> sel;
        sel.addRange(Range<int>(i-1,i));
        table.setSelectedRows(sel);
      }
    }

    void MoveDownSelected()
    {
      int i=table.getSelectedRow();
      if (i<(int)m_patches->size()-1)
      {
        std::swap((*m_patches)[i],(*m_patches)[i+1]);
        SparseSet<int> sel;
        sel.addRange(Range<int>(i+1,i+2));
        table.setSelectedRows(sel);
      }
    }

    void ToggleSelectedLoop()
    {
      int i=table.getSelectedRow();
      if (i!=-1)
      {
        (*m_patches)[i].m_loop=!(*m_patches)[i].m_loop;
        SparseSet<int> sel;
        table.setSelectedRows(sel);
        table.updateContent();
      }
    }


private:
    TableListBox table;     // the table component itself



    std::vector<Patch> *m_patches;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TableDemoComponent)
};



//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class FilePlaybackPluginAudioProcessorEditor  : public AudioProcessorEditor,
                                                public Button::Listener
{
public:
    //==============================================================================
    FilePlaybackPluginAudioProcessorEditor (AudioProcessor& processor);
    ~FilePlaybackPluginAudioProcessorEditor();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.


    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<TextButton> m_addButton;
    std::unique_ptr<TextButton> m_deleteButton;
    std::unique_ptr<TextButton> m_moveUp;
    std::unique_ptr<TextButton> m_moveDown;
    std::unique_ptr<TableDemoComponent> m_table;
    std::unique_ptr<TextButton> m_toggleLoop;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilePlaybackPluginAudioProcessorEditor)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

