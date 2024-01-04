/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 7.0.2

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "SoundFontPlayerProcessor.h"


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
        table.getHeader().addColumn ("SF2 File",2,330);

        //table.setMultipleSelectionEnabled (true);
        table.selectRow(0);
    }

    // This is overloaded from TableListBoxModel, and must return the total number of rows in our table
    int getNumRows() override
    {
        return 127;
    }

    void SetList(std::vector<std::string> *patches) { m_patches=patches; }

    // This is overloaded from TableListBoxModel, and should fill in the background of the whole row
    void paintRowBackground (Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) override
    {
        if (rowIsSelected)
            g.fillAll (Colours::lightblue);
        else
            g.fillAll (Colours::white);
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
        text=(*m_patches)[rowNumber].c_str();

      g.drawText (text, 2, 0, width - 4, height, Justification::centredLeft, true);

    }

    void resized() override
    {
        // position our table with a gap around its edge
        table.setBoundsInset (BorderSize<int> (8));
    }

    void AddItem(const char*newItem)
    {
        int i = table.getSelectedRow();
      (*m_patches)[i] = newItem;
      SparseSet<int> sel;
      table.setSelectedRows(sel);
      table.updateContent();
      table.selectRow(i);

    }

    void DeleteSelected()
    {
      int i=table.getSelectedRow();
      if (i!=-1)
      {
          ((*m_patches)[i]) = "";
          SparseSet<int> sel;
          table.setSelectedRows(sel);
        table.updateContent();
        table.selectRow(i);

      }
    }


private:
    TableListBox table;     // the table component itself



    std::vector<std::string> *m_patches;

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
class SoundFontPlayerEditor  : public AudioProcessorEditor,
                               public juce::Button::Listener
{
public:
    //==============================================================================
    SoundFontPlayerEditor (AudioProcessor& processor);
    ~SoundFontPlayerEditor() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.


    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::TextButton> m_addButton;
    std::unique_ptr<juce::TextButton> m_deleteButton;
    std::unique_ptr<TableDemoComponent> m_table;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoundFontPlayerEditor)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

