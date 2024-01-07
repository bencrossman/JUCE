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
#include "WavStreamerProcessor.h"


class WavStreamerTableComponent    : public Component, public TableListBoxModel
{
public:
    WavStreamerTableComponent()
    {
        m_patches=NULL;
        // Create our table component and add it to this component..
        addAndMakeVisible (table);
        table.setModel (this);

        table.getHeader().addColumn ("PC",1,30);
        table.getHeader().addColumn ("Wav file",2,330);
        table.getHeader().addColumn ("Mode",3,40);

        //table.setMultipleSelectionEnabled (true);
        table.selectRow(0);
    }

    // This is overloaded from TableListBoxModel, and must return the total number of rows in our table
    int getNumRows() override
    {
        return 127;
    }

    void SetList(std::vector<WavStreamerAudioProcessor::Patch> *patches) { m_patches=patches; }

    // This is overloaded from TableListBoxModel, and should fill in the background of the whole row
    void paintRowBackground (Graphics& g, int, int /*width*/, int /*height*/, bool rowIsSelected) override
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
        text=(*m_patches)[rowNumber].m_file.c_str();
      else if (columnId == 3)
      {
          switch ((*m_patches)[rowNumber].m_mode)
          {
          case WavStreamerAudioProcessor::MODE_NORMAL:text = "normal"; break;
          case WavStreamerAudioProcessor::MODE_LOOP:text = "loop"; break;
          case WavStreamerAudioProcessor::MODE_ONNOTE:text = "onnote"; break;

          }
      }
      g.drawText (text, 2, 0, width - 4, height, Justification::centredLeft, true);

    }

    void resized() override
    {
        // position our table with a gap around its edge
        table.setBoundsInset (BorderSize<int> (8));
    }

    void SetSelected(const char*newItem)
    {
        int i = table.getSelectedRow();
      (*m_patches)[i].m_file = newItem;
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
          ((*m_patches)[i]).m_file = "";
          SparseSet<int> sel;
          table.setSelectedRows(sel);
        table.updateContent();
        table.selectRow(i);

      }
    }

    void ToggleSelectedLoop()
    {
      int i=table.getSelectedRow();
      if (i!=-1)
      {
        (*m_patches)[i].m_mode =((*m_patches)[i].m_mode + 1) % WavStreamerAudioProcessor::MODE_COUNT;
        SparseSet<int> sel;
        table.setSelectedRows(sel);
        table.updateContent();
        table.selectRow(i);
      }
    }


private:
    TableListBox table;     // the table component itself



    std::vector<WavStreamerAudioProcessor::Patch> *m_patches;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavStreamerTableComponent)
};



//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class WavStreamerEditor  : public AudioProcessorEditor,
                           public juce::Button::Listener
{
public:
    //==============================================================================
    WavStreamerEditor (AudioProcessor& processor);
    ~WavStreamerEditor() override;

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
    std::unique_ptr<juce::TextButton> m_setButton;
    std::unique_ptr<juce::TextButton> m_clearButton;
    std::unique_ptr<WavStreamerTableComponent> m_table;
    std::unique_ptr<juce::TextButton> m_cycleButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavStreamerEditor)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

