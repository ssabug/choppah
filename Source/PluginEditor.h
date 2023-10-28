/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include <juce_gui_extra/juce_gui_extra.h>
#include <filesystem>
#include <fstream>

//==============================================================================

class ChopperAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                     public juce::Timer
{
public:
    class controlColorTemplate 
    {
        public:
            void init (int ref,juce::Colour color,std::string templateName="None") { this->colorId = ref; this->colorValue=color; if(templateName!="None") {this->templateName=templateName;}}
            std::string templateName="None";
            int colorId=juce::Label::textColourId;
            juce::Colour colorValue=juce::Colour(juce::Colours::white);
                       
    };
    class Control
    {
         int a;   
    };

    ChopperAudioProcessorEditor (ChopperAudioProcessor&);
    ~ChopperAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;    
    void timerCallback() override;
    void reloadSkin();
    
    int bitArrayToInt32(bool arrr[], int count);
    std::vector<std::string> split(std::string s, std::string delimiter);
    int invert_int(int toBeInverted);
    std::string removeForbiddenCharacters(std::string text);
    
    void step_seq_update(int stepIndex);
    void step_seq_click(int stepIndex);
    void step_seq_clear();
    void step_seq_reset();
    void step_seq_copy(); 
    void step_seq_paste();
    void step_seq_change();    
    void seq_seq_change(int step); 
    void seq_pattern_change(); 
    void seq_env_change(); 
    void seq_length_change(); 
    void out_gain_change();
    void out_mix_change();
    void seq_mode_change();
    void seq_clock_change();
    void seq_gate_length_change();
    void seq_auto_click();
    void ui_debug(std::string text);
    void init_all_sequences();
    void preset_change();
    void save_preset();
    void load_config();

    std::vector<ChopperAudioProcessorEditor::controlColorTemplate> getSkinColorsFromXML();
    void LoadXMLConfig(bool reloadSkin,bool reloadPatternsAndSequences,bool reloadParameters);
    void writeXMLConfig(bool updateSkin,bool updatePatternsAndSequences,bool updateParameters);
    void writeXMLParam(std::string xmlFilePath,std::string paramPath,std::string paramWriteValue);
    std::string readXMLParam(std::string xmlFilePath,std::string paramPath);
    void loadXMLPreset(std::string presetPath);
    void writeXMLPreset(std::string presetName);
    juce::XmlElement* getXMLDef_PluginParameters();
    juce::XmlElement* getXMLDef_Patterns();
    juce::XmlElement* getXMLDef_Sequences();

    bool presetExists(std::string presetText);
    std::vector<std::string> getPresetList();
    void initDirectories();  
    std::vector<std::string> get_directories(const std::string& s);

    void debugF(bool reloadFromFile); 
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ChopperAudioProcessor& audioProcessor;

    std::unique_ptr<juce::Slider> mix__slider;
    std::unique_ptr<juce::Slider> gain__slider;
    //juce::Slider mix__slider;
    //juce::Slider gain__slider;
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

    juce::ComboBox preset_sel;
    juce::Label preset_sel_label;
    juce::TextButton preset_save,config_load;
    juce::Label preset_save_label,config_load_label;

    juce::ImageButton banner;
    juce::TextEditor debug;
    juce::TextButton debugB,debugBB;

    const int gui_refresh_rate=500;

    std::string imagePath,dataPath,configPath,presetPath,currentSkin="default",currentSkinPath;
    juce::Image stepseq_on,stepseq_off,plugin_banner;
    long int stepseq_color=0xFF0000FF,background_color=0xFF000000;
    
    bool patterns[16][16]/*={{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0},{1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0},
                           {1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0},{1,0,0,0,1,0,1,0,1,1,1,0,1,0,1,0},{1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0},{1,1,0,0,1,1,1,1,1,1,0,0,1,1,1,1},
                           {0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1},{0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1},{0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1},{0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},
                           {0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0},{0,0,1,1,0,0,0,1,1,0,1,1,0,0,0,1},{1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} 
                            }*/;
    int sequences[16][16]/*={{0,1,0,2,0,1,0,2,0,1,0,2,0,1,0,2},{3,4,3,5,3,4,3,5,3,4,3,5,3,4,3,5},{6,7,6,8,6,7,6,8,6,7,6,8,6,7,6,8},{0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},
                           {0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},{0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},{0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},{0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},
                           {0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},{0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},{0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},{0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},
                           {0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},{0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},{0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6},{0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6}            
                            }*/;
    int ssequence[16]={0,1,0,2,4,5,4,6,0,4,3,5,0,2,0,6};
    bool sequence[16];
    bool sequence_clipboard[16];

    void initSlider1(std::string name,std::unique_ptr<juce::Slider>& slider,std::vector<controlColorTemplate> slider_colors,int x,int y,int w,int h,float min,float max,float def);
    void initSlider2(juce::Slider& slider,std::vector<controlColorTemplate> slider_colors,int x,int y,int w,int h,float min,float max,float def);    
    void initCombo1(juce::ComboBox& comboBox,std::vector<controlColorTemplate> combo_colors, int x,int y,int w,int h);
    void initLabel1(juce::Label& label,std::string text,std::vector<controlColorTemplate> label_colors,int x,int y,int w,int h);
    void initUtilityButton(juce::ImageButton& button,std::vector<controlColorTemplate> imagebutton_colors, int x,int y,int w,int h);
    void initStepSeqButton(juce::ImageButton& button,const int index,std::vector<controlColorTemplate> imagebutton_colors, int x,int y,int w,int h); 
    void initPresetCombo(juce::ComboBox& comboBox,std::vector<controlColorTemplate> combo_colors, int x,int y,int w,int h);   
    void initPresetButton(juce::TextButton& button,std::vector<controlColorTemplate> imagebutton_colors, int x,int y,int w,int h);
    
    void switch_skins();
   
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChopperAudioProcessorEditor)
};
