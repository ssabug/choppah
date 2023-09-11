/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <juce_gui_extra/juce_gui_extra.h>
#include <filesystem>


//==============================================================================

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
        juce::Colour step_color_full=juce::Colour (0xFFFFFFFF & stepseq_color);
        juce::Colour step_color_dim=juce::Colour  (0xFF7F7F7F & stepseq_color);
        juce::Colour step_color_dimm=juce::Colour (0xFF3F3F3F & stepseq_color);
        juce::Colour step_color_off=juce::Colour  (0xFF000000 & stepseq_color);

        int clockdiv=seq_clock.getSelectedItemIndex();
        int coeff=pow(2,2+clockdiv);  
        double ppq=audioProcessor.getDAWppq()/coeff;
        double ppqlastbar=audioProcessor.getDAWppqlastbar()/coeff;
        int processorPattern=audioProcessor.pseq_current->get()-1;
        int seqLength=audioProcessor.sseq_length->get();
        int seqClock=audioProcessor.clock_div->getIndex();
        int seqMode=audioProcessor.seq_mode->getIndex(); 
        int autoPattern=audioProcessor.pseq_auto->get();
        int index=std::floor((ppq-ppqlastbar)*16);
        int seqIndex;
        if (ppq>=seqLength) {seqIndex=int(std::floor(ppq)-seqLength*std::floor(ppq/seqLength));} else {seqIndex=std::floor(ppq);}

        /*debug.setText ("ppq = " + std::__cxx11::to_string(ppq)+"\n"+
                       "pattern\t\t\t" + std::__cxx11::to_string(audioProcessor.pseq_current->get())+
                       " step " +std::__cxx11::to_string(index+1)+"\n" +
                       "sequence\t" +std::__cxx11::to_string(seq_sequence_selected.getSelectedItemIndex()+1)+ 
                       " step " +std::__cxx11::to_string(seqIndex+1) +"\n"/*+
                       "toto ="+ toto +"\n"+
                        "tata : " + tata
                       , juce::dontSendNotification);  */

        for (int i=0;i<sizeof(sequence)/sizeof(sequence[0]);i++)
        {
            juce::Colour color,color_seq;
            if (i == index){color=step_color_full;} else {color=step_color_dim;}
            if (i == seqIndex){color_seq=step_color_full;} else if (i>=seqLength) {color_seq=step_color_off;} else {color_seq=step_color_dimm;}
            
            if (seq_pattern_selected.getSelectedItemIndex() !=  processorPattern ) {
                sequence[i]=patterns[processorPattern][i];
            }

            if (sequence[i]==true)
            {
                seq_step[i].setImages (false, true, true,stepseq_on, 1.000f, color,juce::Image(), 1.000f, step_color_full,stepseq_off, 1.000f, color);
            }
            else
            {
                seq_step[i].setImages (false, true, true,stepseq_off, 1.000f, color,juce::Image(), 1.000f, step_color_full,stepseq_on, 1.000f, color);
            }
            
            sseq_step[i].setColour(juce::ComboBox::outlineColourId ,color_seq);
        }

        if (autoPattern == 1) {
            seq_auto.setImages (false, true, true,stepseq_on, 1.000f, step_color_dim,juce::Image(), 1.000f, step_color_full,stepseq_off, 1.000f, step_color_dim);
        } else {
            seq_auto.setImages (false, true, true,stepseq_off, 1.000f, step_color_dim,juce::Image(), 1.000f, step_color_full,stepseq_on, 1.000f, step_color_dim);
        }
        
        if (seq_clock.getSelectedItemIndex() != seqClock) {
            seq_clock.setSelectedItemIndex(seqClock,juce::dontSendNotification);
        }
        if (seq_mode.getSelectedItemIndex() != seqMode) {
            seq_mode.setSelectedItemIndex(seqMode,juce::dontSendNotification);
        }
        if (seq_pattern_selected.getSelectedItemIndex() !=  processorPattern && seqMode == 1) {
            seq_pattern_selected.setSelectedItemIndex(processorPattern);
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
        juce::Colour step_color_full=juce::Colour (0xFFFFFFFF& stepseq_color);
        juce::Colour step_color_dim=juce::Colour (0xFF7F7F7F& stepseq_color);
        
        if ( sequence[stepIndex] == true)        
        {
            seq_step[stepIndex].setImages (false, true, true,stepseq_on, 1.000f, step_color_dim,juce::Image(), 1.000f, step_color_full,stepseq_off, 1.000f, step_color_dim);
        } else 
        {
            seq_step[stepIndex].setImages (false, true, true,stepseq_off, 1.000f, step_color_dim,juce::Image(), 1.000f, step_color_full,stepseq_on, 1.000f, step_color_dim);
        }
        patterns[seq_pattern_selected.getSelectedItemIndex()][stepIndex]=sequence[stepIndex];
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
        
        for (int i=0;i<sizeof(sequence)/sizeof(sequence[0]);i++) 
            {
                sequence[i]=patterns[seqIndex][i];
                step_seq_update(i);
            }
        audioProcessor.pseq_current->operator=(seqIndex+1);
    }
    void seq_seq_change(int step) 
    {   
        int seqIndex=seq_sequence_selected.getSelectedItemIndex();
        sequences[seqIndex][step]=sseq_step[step].getSelectedItemIndex();       
        //ssequence[step]=sequences[seqIndex][step];
        audioProcessor.pattern_seqs[seqIndex][step]=sseq_step[step].getSelectedItemIndex();
        
        //audioProcessor.sseq_current->operator=(seqIndex+1);
    }
    void seq_pattern_change() 
    {   
        int seqIndex=seq_sequence_selected.getSelectedItemIndex();  
        for (int i=0;i<sizeof(sequence)/sizeof(sequence[0]);i++) 
            {
                //ssequence[i]=sequences[seqIndex][i];
                sseq_step[i].setSelectedItemIndex(sequences[seqIndex][i]);
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
    void ui_debug(std::string text)
    {
        debug.setText (text, juce::dontSendNotification);
    }
    void init_all_sequences()
    {
        for (int i=15;i>=0;i--){
            //audioProcessor.sequences[i]=bitArrayToInt32(patterns[i],16);
            seq_pattern_selected.setSelectedItemIndex(i);
            for (int j;j<16;j++){
                audioProcessor.pattern_seqs[i][j]=patterns[i][j];
            }
            
            audioProcessor.updatePattern(i,bitArrayToInt32(patterns[i],16));
        }
        
        //audioProcessor.initGatesMap();
    }

    void seq_auto_click()
    {
        int value=0;
        if ( audioProcessor.pseq_auto->get() == 0) {
            value=1;
        } else {
            value=0;
        }
        audioProcessor.pseq_auto->operator=(value);
    }
    
    void LoadXMLConfig();
    void initDirectories();  
    void reloadGui(); 

    class controlColorTemplate 
    {
        public:
            void init (int ref,juce::Colour color) { this->colorId = ref; this->colorValue=color;}
            int colorId=juce::Label::textColourId;
            juce::Colour colorValue=juce::Colour(juce::Colours::white);
                       
    };

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
    juce::ImageButton seq_auto;
    juce::Label seq_auto_label;

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
    juce::TextButton debugB;

    const int gui_refresh_rate=500;

    std::string imagePath,dataPath,configPath,presetPath,currentSkin;
    juce::Image stepseq_on,stepseq_off,plugin_banner;
    long int stepseq_color=0xFF0000FF;
    
    bool patterns[16][16]={{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0},{1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0},
                           {1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0},{1,0,0,0,1,0,1,0,1,1,1,0,1,0,1,0},{1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0},{1,1,0,0,1,1,1,1,1,1,0,0,1,1,1,1},
                           {0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1},{0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1},{0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1},{0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},
                           {0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0},{0,0,1,1,0,0,0,1,1,0,1,1,0,0,0,1},{1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} 
                            };
    int sequences[16][16]={{0,1,0,2,0,1,0,2,0,1,0,2,0,1,0,2},{3,4,3,5,3,4,3,5,3,4,3,5,3,4,3,5},{6,7,6,8,6,7,6,8,6,7,6,8,6,7,6,8},{0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},
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
