/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <juce_gui_extra/juce_gui_extra.h>

//==============================================================================
/**
*/
/*class MySliderListener : public juce::Slider::Listener
{
public:
    void sliderValueChanged(juce::Slider* slider) override
    {
        // Handle the value change here
        double value = slider->getValue();
        // Use the value in the PluginProcessor part
    }
};*/


class ChopperAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                     public juce::Timer
{
public:
    ChopperAudioProcessorEditor (ChopperAudioProcessor&);
    ~ChopperAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override
    {
        int seqword=bitArrayToInt32(sequence,16);
        int clockdiv=seq_clock.getSelectedItemIndex();
        int coeff=pow(2,2+clockdiv);
        long int bar=audioProcessor.getDAWbarcount()/coeff;
        double ppq=audioProcessor.getDAWppq()/coeff;
        double ppqlastbar=audioProcessor.getDAWppqlastbar()/coeff;
        int index=std::floor((ppq-ppqlastbar)*16);
        int seqIndex;
        int seqLength=audioProcessor.sseq_length->get();
        int seqClock=audioProcessor.clock_div->getIndex();
        int seqMode=audioProcessor.seq_mode->getIndex();
        if (ppq>=seqLength) {seqIndex=int(std::floor(ppq)-seqLength*std::floor(ppq/seqLength));} else {seqIndex=std::floor(ppq);}
        //unsigned int seqStepValue=(seqword >> int(std::floor((ppq-ppqlastbar)*16))) & 1;
        unsigned int seqStepValue=int(std::floor((ppq-ppqlastbar)*64));
        int processorPattern=audioProcessor.pseq_current->get()-1;

        debug.setText ("ppq = " + std::__cxx11::to_string(ppq)+"\n"+
                       "pattern\t\t\t" + std::__cxx11::to_string(seq_pattern_selected.getSelectedItemIndex()+1)+
                       " step " +std::__cxx11::to_string(index+1)+"\n" +
                       "sequence\t" +std::__cxx11::to_string(seq_sequence_selected.getSelectedItemIndex()+1)+ 
                       " step " +std::__cxx11::to_string(seqIndex+1)
                       , juce::dontSendNotification);      

        for (int i=0;i<sizeof(sequence)/sizeof(sequence[0]);i++)
        {
            int color,color_seq;
            if (i == index){color=0xFF0000FF;} else {color=0xFF00007F;}
            if (i == seqIndex){color_seq=0xFF0000FF;} else if (i>=seqLength) {color_seq=0xFF000000;} else {color_seq=0xFF00003F;}
            
            if (seq_pattern_selected.getSelectedItemIndex() !=  processorPattern ) {
                sequence[i]=patterns[processorPattern][i];
            }

            if (sequence[i]==true)
            {
                seq_step[i].setImages (false, true, true,
                                  juce::ImageFileFormat::loadFrom(juce::File(imagePath+"/stepseq_on.png")), 1.000f, juce::Colour (color),
                                  juce::Image(), 1.000f, juce::Colour (0xFF0000FF),
                                  juce::ImageFileFormat::loadFrom(juce::File(imagePath+"/stepseq_off.png")), 1.000f, juce::Colour (color));
            }
            else
            {
                seq_step[i].setImages (false, true, true,
                                  juce::ImageFileFormat::loadFrom(juce::File(imagePath+"/stepseq_off.png")), 1.000f, juce::Colour (color),
                                  juce::Image(), 1.000f, juce::Colour (0xFF0000FF),
                                  juce::ImageFileFormat::loadFrom(juce::File(imagePath+"/stepseq_on.png")), 1.000f, juce::Colour (color));
            }
            
            sseq_step[i].setColour(juce::ComboBox::outlineColourId ,juce::Colour (color_seq));
        }
        
        if (seq_clock.getSelectedItemIndex() != seqClock) {
            seq_clock.setSelectedItemIndex(seqClock,juce::dontSendNotification);
        }
        if (seq_mode.getSelectedItemIndex() != seqMode) {
            seq_mode.setSelectedItemIndex(seqMode,juce::dontSendNotification);
        }
        if (seq_pattern_selected.getSelectedItemIndex() !=  processorPattern ) {
            seq_pattern_selected.setSelectedItemIndex(processorPattern,juce::dontSendNotification);
        }
        
    }
    int bitArrayToInt32(bool arrr[], int count)
    {
        int ret = 0;
        int tmp;
        bool arr[count];
        for (int i = 0; i < count; i++) {
            arr[i]=arrr[count-1-i];
        }
        
        for (int i = 0; i < count; i++) {
            tmp = arr[i];
            ret |= tmp << (count - i - 1);
        }
        return ret;
    }
    void step_seq_update(int stepIndex)
    {
        
        if ( sequence[stepIndex] == true) 
        
        {
            seq_step[stepIndex].setImages (false, true, true,
                                  juce::ImageFileFormat::loadFrom(juce::File(imagePath+"/stepseq_on.png")), 1.000f, juce::Colour (0xFF00007F),
                                  juce::Image(), 1.000f, juce::Colour (0xFF0000FF),
                                  juce::ImageFileFormat::loadFrom(juce::File(imagePath+"/stepseq_off.png")), 1.000f, juce::Colour (0xFF00007F));
        } else 
        {
            seq_step[stepIndex].setImages (false, true, true,
                                  juce::ImageFileFormat::loadFrom(juce::File(imagePath+"/stepseq_off.png")), 1.000f, juce::Colour (0xFF00007F),
                                  juce::Image(), 1.000f, juce::Colour (0xFF0000FF),
                                  juce::ImageFileFormat::loadFrom(juce::File(imagePath+"/stepseq_on.png")), 1.000f, juce::Colour (0xFF00007F));
        }
        patterns[seq_pattern_selected.getSelectedItemIndex()][stepIndex]=sequence[stepIndex];
        //debug_label.setText (std::__cxx11::to_string(bitArrayToInt32(sequence,16)), juce::dontSendNotification);
        /*long int bar=audioProcessor.getDAWppqbar()/4;
        if (bar>=4){
            bar=std::floor(bar/(std::floor(bar/4)));
        }*/
        audioProcessor.updatePattern(seq_pattern_selected.getSelectedItemIndex(),bitArrayToInt32(sequence,16));
        
       
    }
    void step_seq_click(int stepIndex)
    {
        if ( sequence[stepIndex] == false) { sequence[stepIndex]=true;} else {sequence[stepIndex]=false;}
        
        step_seq_update(stepIndex);      
    }
    void step_seq_clear() 
    {
        for (int i=0;i<sizeof(sequence)/sizeof(sequence[0]);i++) {
            sequence[i]=false;
            step_seq_update(i);
        }
    }
    void step_seq_reset() 
    {
        for (int j=0;j<sizeof(sequence)/sizeof(sequence[0]);j++) {
            for (int i=0;i<sizeof(sequence)/sizeof(sequence[0]);i++) {
                patterns[j][i]=false;
            }
            sequence[j]=false;
            step_seq_update(j);
        }
        
    }
    void step_seq_copy() 
    {      
        for (int i=0;i<sizeof(sequence)/sizeof(sequence[0]);i++) 
        { 
            sequence_clipboard[i]=sequence[i];
            step_seq_update(i);
        }
    }
    void step_seq_paste() 
    {
        for (int i=0;i<sizeof(sequence)/sizeof(sequence[0]);i++) 
        { 
            sequence[i]=sequence_clipboard[i];
            step_seq_update(i);
        }
    }
    void step_seq_change() 
    { 
        int seqIndex=seq_pattern_selected.getSelectedItemIndex();
        audioProcessor.pseq_current->operator=(seqIndex+1);
        for (int i=0;i<sizeof(sequence)/sizeof(sequence[0]);i++) 
            {
                sequence[i]=patterns[seqIndex][i];
                step_seq_update(i);
            }
    }
    void seq_seq_change(int step) 
    {   
        int seqIndex=seq_sequence_selected.getSelectedItemIndex();
        sequences[seqIndex][step]=sseq_step[step].getSelectedItemIndex();       
        ssequence[step]=sequences[seqIndex][step];
        audioProcessor.pseq_current->operator=(seqIndex+1);
    }
    void seq_pattern_change() 
    {   
        int seqIndex=seq_sequence_selected.getSelectedItemIndex();  
        for (int i=0;i<sizeof(sequence)/sizeof(sequence[0]);i++) 
            {
                ssequence[i]=sequences[seqIndex][i];
                sseq_step[i].setSelectedItemIndex(ssequence[i]);
            }
        audioProcessor.sseq_current->operator=(seqIndex+1);
    }
    void seq_env_change() 
    {
        audioProcessor.updateEnv(seq_env.getSelectedItemIndex());      
    }
    void seq_length_change() 
    {
        audioProcessor.sseq_length->operator=(seq_length.getValue());
    }
    void out_gain_change()
    {
        audioProcessor.out_gain->operator=(gain__slider->getValue());
    }
    void out_mix_change()
    {
        audioProcessor.out_mix->operator=(mix__slider->getValue());
    }
    void seq_mode_change()
    {
        audioProcessor.seq_mode->operator=(seq_mode.getSelectedItemIndex());
    }
    void seq_clock_change()
    {
        audioProcessor.clock_div->operator=(seq_clock.getSelectedItemIndex());
    }
    void seq_gate_length_change()
    {
        audioProcessor.pseq_gate_length->operator=(seq_gate_length.getValue());
    }


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ChopperAudioProcessor& audioProcessor;
    //void sliderValueChanged(juce::Slider* slider) override;

    std::unique_ptr<juce::Slider> mix__slider;
    std::unique_ptr<juce::Slider> gain__slider;
    juce::Label  mix_gain_label;
    juce::Label  mix_amnt_label;

    juce::ImageButton seq_step[16];
    juce::Label seq_step_l[4];
    juce::ComboBox seq_pattern_selected;
    juce::ComboBox sseq_step[16];
    juce::ComboBox seq_sequence_selected;

    juce::ComboBox seq_clock;
    juce::Label  seq_clock_label;
    juce::Slider seq_gate_length;
    juce::Label seq_gate_length_label;
    juce::ComboBox seq_mode;
    juce::Label seq_mode_label;
    juce::ComboBox seq_env;
    juce::Label seq_env_label;
    juce::Slider seq_length;
    juce::Label seq_length_label;

    juce::ImageButton seq_clear;
    juce::Label seq_clear_label;
    juce::ImageButton seq_reset;
    juce::Label seq_reset_label;
    juce::ImageButton seq_copy;
    juce::Label seq_copy_label;
    juce::ImageButton seq_paste;
    juce::Label seq_paste_label;

    juce::ImageButton banner;
    juce::TextEditor debug;
    
    

    const std::string imagePath="/home/pwner/dev/chopper/Ressources/images";

    bool patterns[16][16]={{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0},{1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0},
                           {1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1},{0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1},{1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0},
                           {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
                           {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} 
                            };
    int sequences[16][16]={{0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},{0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},{0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},
                           {0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},{0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},{0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},{0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},
                           {0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},{0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},{0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},{0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},
                           {0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},{0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},{0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},{0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6}            
                            };
    int ssequence[16]={0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6};
    bool sequence[16];
    bool sequence_clipboard[16];
    //juce::SliderParameterAttachment mix__slider_attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChopperAudioProcessorEditor)
};
