/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ChopperAudioProcessorEditor::ChopperAudioProcessorEditor (ChopperAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    //LoadXMLConfig();
    initDirectories();
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1060, 250);

    // VARIABLES THAT MAY BE SHARED WITH PROCESSOR
    int seq_size=16;
    int seq_pattern_count=16;
    int seq_sequence_count=16;

    // POSITIONS
    int bottomy=130;
    int pseq_posx=100;int pseq_posy=20; 
    int sseq_posx=100;int sseq_posy=70;   
    int pseq_params_posx=pseq_posx-80;int pseq_params_posy=pseq_posy+bottomy;
    int mix_posx=840;int mix_posy=pseq_posy+bottomy;
    int mix_label_posx=mix_posx+30;int mix_label_posy=mix_posy-20;
    int util_posx=500;int util_posy=pseq_posy+bottomy;
    int debug_posx=util_posx-210+10;int debug_posy=pseq_posy+bottomy;
    int banner_posx=640;int banner_posy=bottomy-50;
    //COLOURS
    juce::Colour color_text2=juce::Colours::blue;
    juce::Colour color_text1=juce::Colours::blue;
    juce::Colour control_label_color=juce::Colour (0, 0, 200);
    juce::Colour control_value_color=juce::Colour (0, 0, 255);
    juce::Colour background_color=juce::Colours::black;
    int color_stepseq_1=0xFF00007F;
    int color_stepseq_2=0xFF0000FF;
    //IMAGES
    stepseq_off= juce::ImageFileFormat::loadFrom(juce::File(imagePath+"/stepseq_off.png"));
    stepseq_on=juce::ImageFileFormat::loadFrom(juce::File(imagePath+"/stepseq_on.png"));
    plugin_banner=juce::ImageFileFormat::loadFrom(juce::File(imagePath+"/banner_4.png"));

    //////////////////////////////// OUT SETTINGS///////////////////////////////
    gain__slider.reset (new juce::Slider ("mix_gain"));
    gain__slider->setRange (0.0, 10.0, 0.01);
    
    gain__slider->setSliderStyle (juce::Slider::Rotary);
    gain__slider->setTextBoxStyle (juce::Slider::TextBoxBelow, true, 80, 20);
    gain__slider->setBounds (mix_posx, mix_posy, 118, 83);
    gain__slider->setColour(juce::Slider::thumbColourId,juce::Colours::blue);
    gain__slider->setColour(juce::Slider::rotarySliderOutlineColourId,juce::Colours::black);
    gain__slider->setColour(juce::Slider::rotarySliderFillColourId,juce::Colours::blue);
    gain__slider->setColour(juce::Slider::textBoxTextColourId,control_value_color);
    gain__slider->setColour(juce::Slider::textBoxOutlineColourId,juce::Colours::blue);
    gain__slider->onValueChange = [this] { out_gain_change(); };
    gain__slider->setValue (1.0);
    mix_gain_label.setText ("Out Gain", juce::dontSendNotification);
    mix_gain_label.setBounds(mix_label_posx,mix_label_posy,150,24);
    mix_gain_label.setColour(juce::Label::textColourId,control_label_color);
    

    mix__slider.reset (new juce::Slider ("mix_amnt"));
    mix__slider->setRange (0.0, 1.0, 0.01);
    mix__slider->setSliderStyle (juce::Slider::Rotary);
    mix__slider->setTextBoxStyle (juce::Slider::TextBoxBelow, true, 80, 20);
    mix__slider->setBounds (mix_posx+100, mix_posy, 118, 83);
    mix__slider->setColour(juce::Slider::thumbColourId,juce::Colours::blue);
    mix__slider->setColour(juce::Slider::rotarySliderOutlineColourId,juce::Colours::black);
    mix__slider->setColour(juce::Slider::rotarySliderFillColourId,juce::Colours::blue);
    mix__slider->setColour(juce::Slider::textBoxTextColourId,control_value_color);
    mix__slider->setColour(juce::Slider::textBoxOutlineColourId,juce::Colours::blue);
    mix__slider->onValueChange = [this] { out_mix_change(); };
    mix__slider->setValue (1.0);
    mix_amnt_label.setText ("Dry/Wet", juce::dontSendNotification);
    mix_amnt_label.setBounds(mix_label_posx+100,mix_label_posy,150,24);
    mix_amnt_label.setColour(juce::Label::textColourId,control_label_color);
    
    //juce::AudioProcessorParameter::getValueForText("out_mix")
    /*auto& param = processor.getParameter(ChopperAudioProcessor::out_mix);
    auto* attachment = new juce::AudioProcessorValueTreeState::SliderAttachment(processor.getParameterTree(), param, mix__slider);*/

    //////////////////////////////PATTERN SEQ//////////////////////////////////////
    for(int i = 0; i < seq_size;i++) {
        //seq_step[i].setButtonText (TRANS("stepseq"<<i));
        seq_step[i].setToggleable(true);
        seq_step[i].setImages (false, true, true,
                                  stepseq_off, 1.000f, juce::Colour (color_stepseq_1),
                                  juce::Image(), 1.000f, juce::Colour (color_stepseq_2),
                                  stepseq_on, 1.000f, juce::Colour (color_stepseq_1));
        seq_step[i].setBounds (pseq_posx+i*60, pseq_posy, 46, 16);
        addAndMakeVisible (seq_step[i]);
        sequence[i]=false;
    } 
    seq_step[0].onClick = [this] { step_seq_click(0); };seq_step[8].onClick = [this] { step_seq_click(8); };
    seq_step[1].onClick = [this] { step_seq_click(1); };seq_step[9].onClick = [this] { step_seq_click(9); };
    seq_step[2].onClick = [this] { step_seq_click(2); };seq_step[10].onClick = [this] { step_seq_click(10); };
    seq_step[3].onClick = [this] { step_seq_click(3); };seq_step[11].onClick = [this] { step_seq_click(11); };
    seq_step[4].onClick = [this] { step_seq_click(4); };seq_step[12].onClick = [this] { step_seq_click(12); };
    seq_step[5].onClick = [this] { step_seq_click(5); };seq_step[13].onClick = [this] { step_seq_click(13); };
    seq_step[6].onClick = [this] { step_seq_click(6); };seq_step[14].onClick = [this] { step_seq_click(14); };
    seq_step[7].onClick = [this] { step_seq_click(7); };seq_step[15].onClick = [this] { step_seq_click(15); };

    for(int i = 0; i < sizeof(seq_step_l)/sizeof(seq_step_l[0]); i++) {
            seq_step_l[i].setText ("______", juce::dontSendNotification);
            seq_step_l[i].setColour(juce::Label::textColourId,color_text2);
            seq_step_l[i].setBounds (pseq_posx+i*240, pseq_posy+10, 46, 16);
            addAndMakeVisible (seq_step_l[i]);
        }

    seq_pattern_selected.setEditableText (false);
    for(int i = 1; i <= seq_pattern_count; i++) { seq_pattern_selected.addItem (TRANS(std::to_string(i)), i);   }
    seq_pattern_selected.setSelectedItemIndex(0);
    seq_pattern_selected.setColour(juce::Label::textColourId,color_text1);
    seq_pattern_selected.setColour(juce::ComboBox::backgroundColourId,juce::Colours::black);
    seq_pattern_selected.setColour(juce::ComboBox::outlineColourId ,juce::Colours::blue);
    seq_pattern_selected.setColour(juce::ComboBox::arrowColourId ,control_value_color);
    seq_pattern_selected.setColour(juce::ComboBox::textColourId,control_value_color);
    //seq_pattern_selected.addListener (this);
    seq_pattern_selected.setBounds (pseq_posx-85,pseq_posy-3 , 60, 24);
    seq_pattern_selected.onChange = [this] { step_seq_change(); };
    

    //////////////////////////////SEQUENCE SEQ////////////////////////////////////// 
    for(int i = 0; i < seq_size; i++) {
        sseq_step[i].addItem (TRANS("1"), 1);sseq_step[i].addItem (TRANS("2"), 2);sseq_step[i].addItem (TRANS("3"), 3);sseq_step[i].addItem (TRANS("4"), 4);
        sseq_step[i].addItem (TRANS("5"), 5);sseq_step[i].addItem (TRANS("6"), 6);sseq_step[i].addItem (TRANS("7"), 7);sseq_step[i].addItem (TRANS("8"), 8);
        sseq_step[i].addItem (TRANS("9"), 9);sseq_step[i].addItem (TRANS("10"), 10);sseq_step[i].addItem (TRANS("11"), 11);sseq_step[i].addItem (TRANS("12"), 12);
        sseq_step[i].addItem (TRANS("13"), 13);sseq_step[i].addItem (TRANS("14"), 14);sseq_step[i].addItem (TRANS("15"), 15);sseq_step[i].addItem (TRANS("16"), 16);
        sseq_step[i].setSelectedItemIndex(0);
        sseq_step[i].setBounds (sseq_posx-3+i*60, sseq_posy, 53, 16);
        sseq_step[i].setColour(juce::ComboBox::backgroundColourId,juce::Colours::black);
        sseq_step[i].setColour(juce::ComboBox::outlineColourId ,juce::Colours::blue);
        sseq_step[i].setColour(juce::ComboBox::arrowColourId ,control_value_color);
        sseq_step[i].setColour(juce::ComboBox::textColourId,control_value_color);
        addAndMakeVisible (sseq_step[i]);
    }
    sseq_step[0].onChange = [this] { seq_seq_change(0); };sseq_step[4].onChange = [this] { seq_seq_change(4); };sseq_step[8].onChange = [this] { seq_seq_change(8); };sseq_step[12].onChange = [this] { seq_seq_change(12); };
    sseq_step[1].onChange = [this] { seq_seq_change(1); };sseq_step[5].onChange = [this] { seq_seq_change(5); };sseq_step[9].onChange = [this] { seq_seq_change(9); };sseq_step[13].onChange = [this] { seq_seq_change(13); };
    sseq_step[2].onChange = [this] { seq_seq_change(2); };sseq_step[6].onChange = [this] { seq_seq_change(6); };sseq_step[10].onChange = [this] { seq_seq_change(10); };sseq_step[14].onChange = [this] { seq_seq_change(14); };
    sseq_step[3].onChange = [this] { seq_seq_change(3); };sseq_step[7].onChange = [this] { seq_seq_change(7); };sseq_step[11].onChange = [this] { seq_seq_change(11); };sseq_step[15].onChange = [this] { seq_seq_change(15); };
 
    seq_sequence_selected.setEditableText (false);
    seq_sequence_selected.setJustificationType (juce::Justification::centredLeft);
    seq_sequence_selected.setColour(juce::ComboBox::backgroundColourId,juce::Colours::black);
    seq_sequence_selected.setColour(juce::ComboBox::outlineColourId ,juce::Colours::blue);
    seq_sequence_selected.setColour(juce::ComboBox::arrowColourId ,control_value_color);
    seq_sequence_selected.setColour(juce::ComboBox::textColourId,control_value_color);
    seq_sequence_selected.onChange = [this] { seq_pattern_change(); };
    
    for(int i = 1; i <= seq_sequence_count; i++) { seq_sequence_selected.addItem (TRANS(std::to_string(i)), i);   }
    seq_sequence_selected.setSelectedItemIndex(0);
    //seq_sequence_selected.addListener (this);
    seq_sequence_selected.setBounds (sseq_posx-85, sseq_posy-5, 60, 24);

    
    ////////////////////////////// SEQ PARAMS //////////////////////////////////////  
    seq_clock.setEditableText (false);
    seq_clock.setJustificationType (juce::Justification::centredLeft);
    seq_clock.onChange = [this] { seq_clock_change(); };
    seq_clock.addItem (TRANS("1"), 1);
    seq_clock.addItem (TRANS("1/2"), 2);
    seq_clock.addItem (TRANS("1/4"), 3);
    seq_clock.addItem (TRANS("1/8"), 4);
    seq_clock.addItem (TRANS("1/16"), 5);
    seq_clock.addItem (TRANS("1/32"), 6);
    seq_clock.setSelectedItemIndex(0);
    seq_clock.setColour(juce::ComboBox::backgroundColourId,juce::Colours::black);
    seq_clock.setColour(juce::ComboBox::outlineColourId ,juce::Colours::blue);
    seq_clock.setColour(juce::ComboBox::arrowColourId ,juce::Colours::blue);
    seq_clock.setColour(juce::ComboBox::textColourId,control_value_color);
    seq_clock.setBounds (pseq_params_posx, pseq_params_posy, 86, 24);    
    seq_clock_label.setText ("Clock div", juce::dontSendNotification);
    seq_clock_label.setColour(juce::Label::textColourId,control_label_color);
    seq_clock_label.setBounds (pseq_params_posx, pseq_params_posy-20, 86, 24);
       
    seq_gate_length.setRange (0, 100, 1);
    seq_gate_length.onValueChange = [this] { seq_gate_length_change(); };
    seq_gate_length.setValue (100);
    seq_gate_length.setSliderStyle (juce::Slider::Rotary);
    seq_gate_length.setTextBoxStyle (juce::Slider::TextBoxBelow, true, 80, 20);
    seq_gate_length.setBounds (pseq_params_posx+80, pseq_params_posy, 118, 83);
    seq_gate_length.setColour(juce::Slider::thumbColourId,juce::Colours::blue);
    seq_gate_length.setColour(juce::Slider::rotarySliderOutlineColourId,juce::Colours::black);
    seq_gate_length.setColour(juce::Slider::rotarySliderFillColourId,juce::Colours::blue);
    seq_gate_length.setColour(juce::Slider::textBoxTextColourId,control_value_color);
    seq_gate_length.setColour(juce::Slider::textBoxOutlineColourId,juce::Colours::blue);
    seq_gate_length_label.setText ("Gate length", juce::dontSendNotification);
    seq_gate_length_label.setColour(juce::Label::textColourId,control_label_color);
    seq_gate_length_label.setBounds (pseq_params_posx+80+20, pseq_params_posy-20, 86, 24);

    seq_mode.setEditableText (false);
    seq_mode.setJustificationType (juce::Justification::centredLeft);
    seq_mode.addItem (TRANS("pattern"), 1);
    seq_mode.addItem (TRANS("sequence"), 2);
    seq_mode.onChange = [this] { seq_mode_change(); };
    seq_mode.setSelectedItemIndex(0);
    seq_mode.setColour(juce::ComboBox::backgroundColourId,juce::Colours::black);
    seq_mode.setColour(juce::ComboBox::outlineColourId ,juce::Colours::blue);
    seq_mode.setColour(juce::ComboBox::arrowColourId ,juce::Colours::blue);
    seq_mode.setColour(juce::Label::textColourId,control_label_color);
    seq_mode.setColour(juce::ComboBox::textColourId,control_value_color);
    seq_mode.setBounds (pseq_params_posx, pseq_params_posy+50, 86, 24);    
    seq_mode_label.setText ("Mode", juce::dontSendNotification);
    seq_mode_label.setColour(juce::Label::textColourId,control_label_color);
    seq_mode_label.setBounds (pseq_params_posx+20, pseq_params_posy-20+50, 86, 24);

    seq_env.setEditableText (false);
    seq_env.setJustificationType (juce::Justification::centredLeft);
    seq_env.addItem (TRANS("flat"), 1);
    seq_env.addItem (TRANS("sharp"), 2);
    seq_env.addItem (TRANS("tri"), 3);
    seq_env.setSelectedItemIndex(0);
    seq_env.setColour(juce::ComboBox::backgroundColourId,juce::Colours::black);
    seq_env.setColour(juce::ComboBox::outlineColourId ,juce::Colours::blue);
    seq_env.setColour(juce::ComboBox::arrowColourId ,control_value_color);
    //seq_env.setColour(juce::Label::textColourId,color_text1);
    seq_env.setColour(juce::ComboBox::textColourId,control_value_color);
    seq_env.setBounds (pseq_params_posx+100, pseq_params_posy, 86, 24);    
    seq_env_label.setText ("Enveloppe", juce::dontSendNotification);
    seq_env_label.setColour(juce::Label::textColourId,control_label_color);
    seq_env_label.setBounds (pseq_params_posx+100, pseq_params_posy-20, 86, 24);
    seq_env.onChange = [this] { seq_env_change(); };

    seq_length.setRange (2, 16, 1);
    seq_length.setSliderStyle (juce::Slider:: LinearHorizontal );
    seq_length.setTextBoxStyle (juce::Slider::TextBoxBelow, true, 80, 20);
    seq_length.setBounds (pseq_params_posx+100, pseq_params_posy+50, 86, 28);
    seq_length.setColour(juce::Slider::thumbColourId,juce::Colours::blue);
    seq_length.setColour(juce::Slider::rotarySliderOutlineColourId,juce::Colours::black);
    seq_length.setColour(juce::Slider::rotarySliderFillColourId,juce::Colours::blue);
    seq_length.setColour(juce::Slider::textBoxTextColourId,control_value_color);
    seq_length.setColour(juce::Slider::textBoxOutlineColourId,juce::Colours::black);
    seq_length.onValueChange= [this] { seq_length_change(); };
    seq_length.setValue(4);
    seq_length_label.setText ("Seq length", juce::dontSendNotification);
    seq_length_label.setColour(juce::Label::textColourId,control_label_color);
    seq_length_label.setBounds (pseq_params_posx+100, pseq_params_posy-20+50, 86, 24);

    seq_auto.setToggleable(true);
    seq_auto.setImages (false, true, true,
                              stepseq_off, 1.000f, juce::Colour (color_stepseq_1),
                                  juce::Image(), 1.000f, juce::Colour (color_stepseq_2),
                                  stepseq_on, 1.000f, juce::Colour (color_stepseq_1));
    seq_auto.setBounds (pseq_params_posx+200, pseq_params_posy, 46, 16);
    seq_auto.onClick = [this] { seq_auto_click();};

    //seq_clear.addListener (this);
    seq_auto_label.setColour(juce::Label::textColourId,control_label_color);
    seq_auto_label.setText ("Auto", juce::dontSendNotification);
    seq_auto_label.setBounds (pseq_params_posx+200, pseq_params_posy-20, 86, 24);

    ////////////////////////////// UTILITY ////////////////////////////////////// 
    seq_clear.setToggleable(true);
    seq_clear.setImages (false, true, true,
                              stepseq_off, 1.000f, juce::Colour (color_stepseq_1),
                                  juce::Image(), 1.000f, juce::Colour (color_stepseq_2),
                                  stepseq_on, 1.000f, juce::Colour (color_stepseq_1));
    seq_clear.setBounds (util_posx, util_posy, 46, 16);
    seq_clear.onClick = [this] { step_seq_clear();};
    //seq_clear.addListener (this);
    seq_clear_label.setColour(juce::Label::textColourId,control_label_color);
    seq_clear_label.setText ("clear", juce::dontSendNotification);
    seq_clear_label.setBounds (util_posx, util_posy-20, 86, 24);

    seq_reset.setToggleable(true);
    seq_reset.setImages (false, true, true,
                              stepseq_off, 1.000f, juce::Colour (color_stepseq_1),
                                  juce::Image(), 1.000f, juce::Colour (color_stepseq_2),
                                  stepseq_on, 1.000f, juce::Colour (color_stepseq_1));
    seq_reset.setBounds (util_posx, util_posy+40, 46, 16);
    seq_reset.onClick = [this] { step_seq_reset();};
    //seq_reset.addListener (this);
    seq_reset_label.setColour(juce::Label::textColourId,control_label_color);
    seq_reset_label.setText ("reset", juce::dontSendNotification);
    seq_reset_label.setBounds (util_posx, util_posy+40-20, 86, 24);

    seq_copy.setToggleable(true);
    seq_copy.setImages (false, true, true,
                              stepseq_off, 1.000f, juce::Colour (color_stepseq_1),
                                  juce::Image(), 1.000f, juce::Colour (color_stepseq_2),
                                  stepseq_on, 1.000f, juce::Colour (color_stepseq_1));
    seq_copy.setBounds (util_posx+60, util_posy, 46, 16);
    seq_copy.onClick = [this] { step_seq_copy();};
    //seq_copy.addListener (this);
    seq_copy_label.setColour(juce::Label::textColourId,control_label_color);
    seq_copy_label.setText ("copy", juce::dontSendNotification);
    seq_copy_label.setBounds (util_posx+60, util_posy-20, 86, 24);

    seq_paste.setToggleable(true);
    seq_paste.setImages (false, true, true,
                              stepseq_off, 1.000f, juce::Colour (color_stepseq_1),
                                  juce::Image(), 1.000f, juce::Colour (color_stepseq_2),
                                  stepseq_on, 1.000f, juce::Colour (color_stepseq_1));
    seq_paste.setBounds (util_posx+60, util_posy+40, 46, 16);
    seq_paste.onClick = [this] { step_seq_paste();};
    //seq_paste.addListener (this);
    seq_paste_label.setColour(juce::Label::textColourId,control_label_color);
    seq_paste_label.setText ("paste", juce::dontSendNotification);
    seq_paste_label.setBounds (util_posx+60, util_posy+40-20, 86, 24);
    /////////////////////////////////////BANNER///////////////////////////////////////
    banner.setToggleable(true);
    banner.setImages (false, true, true,
                              plugin_banner, 1.000f,juce::Colour (color_stepseq_1),
                                  juce::Image(), 1.000f, control_label_color,
                                  juce::Image(), 1.000f, control_value_color);
    banner.setBounds (banner_posx,banner_posy+30, 190, 120);
    //banner.onClick = [this] { step_seq_paste();};



    //////////////////////////////////////DEBUG///////////////////////////////////////
    debug.setColour(juce::TextEditor::textColourId,control_value_color);
    //debug.setText ("debug", juce::dontSendNotification);
    debug.setBounds (debug_posx, debug_posy-20, 170, 100);
    debug.setColour(juce::TextEditor::backgroundColourId,background_color);
    debug.setColour(juce::TextEditor::outlineColourId,background_color);
    debug.setColour(juce::TextEditor::shadowColourId,background_color);
    debugB.setBounds (debug_posx-20, debug_posy-20, 15, 15);
    debugB.onClick = [this] { reloadGui();};
    //debug.setScrollbarsShown(true);

    init_all_sequences();

    //////////////////////////////////////////////////////////////////////////////////
    addAndMakeVisible (gain__slider.get());
    addAndMakeVisible (mix_gain_label);
    addAndMakeVisible (mix__slider.get());
    addAndMakeVisible (mix_amnt_label);
    addAndMakeVisible (seq_pattern_selected);
    addAndMakeVisible (seq_sequence_selected);
    addAndMakeVisible (seq_clock);
    addAndMakeVisible (seq_clock_label);
    //addAndMakeVisible (seq_gate_length);
    //addAndMakeVisible (seq_gate_length_label);
    addAndMakeVisible (seq_length);
    addAndMakeVisible (seq_length_label);
    addAndMakeVisible (seq_auto);
    addAndMakeVisible (seq_auto_label);
    addAndMakeVisible (seq_env);
    addAndMakeVisible (seq_env_label);
    addAndMakeVisible (seq_mode);
    addAndMakeVisible (seq_mode_label);
    addAndMakeVisible (seq_clear);
    addAndMakeVisible (seq_clear_label);
    addAndMakeVisible (seq_reset);
    addAndMakeVisible (seq_reset_label);
    addAndMakeVisible (seq_copy);
    addAndMakeVisible (seq_copy_label);
    addAndMakeVisible (seq_paste);
    addAndMakeVisible (seq_paste_label);
    addAndMakeVisible (banner);
    addAndMakeVisible (debug);
    addAndMakeVisible (debugB);
    startTimerHz(gui_refresh_rate);
}


ChopperAudioProcessorEditor::~ChopperAudioProcessorEditor()
{
}

//==============================================================================
void ChopperAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (background_color) );
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);

    g.setColour (juce::Colour (0xFF323232 & stepseq_color));
    int y2=90+30;
    juce::Rectangle<int> psqArea (5,5,1050,50);
    juce::Rectangle<int> spsqArea (5,5,85,50);
    juce::Rectangle<int> ssqArea (5,53,1050,50);
    juce::Rectangle<int> sssqArea (5,53,85,50);
    juce::Rectangle<int> parArea (5,y2,/*218*/275,120);
    juce::Rectangle<int> utilArea (485,y2,138,120);
    juce::Rectangle<int> outArea (845,y2,210,120);
    juce::Rectangle<int> dbgArea (275+20,y2,180,120);

    g.drawRect (psqArea, 2);
    g.drawRect (ssqArea, 2);
    g.drawRect (spsqArea, 2);
    g.drawRect (sssqArea, 2);
    g.drawRect (parArea, 2);
    g.drawRect (utilArea, 2);
    g.drawRect (outArea, 2);
    g.drawRect (dbgArea, 2);

    
}

void ChopperAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void ChopperAudioProcessorEditor::reloadGui()
{
    //COLOURS
    controlColorTemplate slider_1_colors[5];
    controlColorTemplate combo_1_colors[4];
    controlColorTemplate textbox_1_colors[4];
    controlColorTemplate label_1_colors[1];
    controlColorTemplate imagebutton_1_colors[2];
    /*
    juce::Colour color_text2=juce::Colours::blue;
    juce::Colour color_text1=juce::Colours::blue;
    juce::Colour control_label_color=juce::Colour (0, 0, 200);
    juce::Colour control_value_color=juce::Colour (0, 0, 255);
    juce::Colour background_color=juce::Colours::black;
    juce::Colour color_stepseq_1=juce::Colour (0xFF00007F);
    juce::Colour color_stepseq_2=juce::Colour (0xFF0000FF); 
    slider_1_colors[0].init(juce::Slider::thumbColourId,juce::Colours::blue);
    slider_1_colors[1].init(juce::Slider::rotarySliderOutlineColourId,juce::Colours::black);
    slider_1_colors[2].init(juce::Slider::rotarySliderFillColourId,juce::Colours::blue);
    slider_1_colors[3].init(juce::Slider::textBoxTextColourId,control_value_color);
    slider_1_colors[4].init(juce::Slider::textBoxOutlineColourId,juce::Colours::blue);    
    combo_1_colors[0].init(juce::ComboBox::backgroundColourId,juce::Colours::black);
    combo_1_colors[1].init(juce::ComboBox::outlineColourId ,juce::Colours::blue);
    combo_1_colors[2].init(juce::ComboBox::arrowColourId ,control_value_color);
    combo_1_colors[3].init(juce::ComboBox::textColourId,control_value_color);    
    textbox_1_colors[0].init(juce::TextEditor::textColourId,control_value_color);
    textbox_1_colors[1].init(juce::TextEditor::backgroundColourId,background_color);
    textbox_1_colors[2].init(juce::TextEditor::outlineColourId,background_color);
    textbox_1_colors[3].init(juce::TextEditor::shadowColourId,background_color);   
    label_1_colors[0].init(juce::Label::textColourId,control_label_color);   
    imagebutton_1_colors[0].init(juce::Label::textColourId,color_stepseq_1);
    imagebutton_1_colors[1].init(juce::Label::textColourId,color_stepseq_2);*/
    
    LoadXMLConfig(1);

    // LOAD SKIN XML DEF
    std::string xmlFilePath=dataPath+"/skins/"+currentSkin+"/skin.xml"; 
    if ( not std::filesystem::exists(std::string(xmlFilePath)) )  {
        
    } else {
        juce::File xmlFile(xmlFilePath);
        //juce::ScopedPointer<juce::XmlElement> xml(juce::XmlDocument::parse(xmlFile));
        juce::XmlDocument xmlDoc(xmlFile);

        if (juce::XmlDocument::parse(xmlFile))
        {
            // Access the root element of the XML document
            auto rootElement = xmlDoc.getDocumentElement();
            if (rootElement->hasTagName("skin")) {
                for (auto* e : rootElement->getChildByName("templates")->getChildIterator())
                    {   
                        if (e->getTagName() == "background") {
                            int i=0;
                            std::string id,colorHexa;
                            for (auto* f : e->getChildByName("colors")->getChildIterator()) {                              
                                for (auto* g : f->getChildIterator()) {
                                    const juce::String toto=g->getAllSubText();
                                    if (g->getTagName() == "id") {id=toto.toStdString();}
                                    if (g->getTagName() == "color") {colorHexa=toto.toStdString();}
                                }
                                // BACKGROUND 
                                background_color=std::stoul(colorHexa,nullptr,16);
                                i++;
                            }
                        }                                            
                        if (e->getTagName() == "slider1") {
                            int i=0;
                            std::string id,colorHexa;
                            for (auto* f : e->getChildByName("colors")->getChildIterator()) {                              
                                for (auto* g : f->getChildIterator()) {
                                    const juce::String toto=g->getAllSubText();
                                    if (g->getTagName() == "id") {id=toto.toStdString();}
                                    if (g->getTagName() == "color") {colorHexa=toto.toStdString();}
                                }
                                slider_1_colors[i].init(std::stoi(id,nullptr,16),juce::Colour(std::stoul(colorHexa,nullptr,16)) );
                                i++;
                            }
                        }
                        if (e->getTagName() == "combo1") {
                            int i=0;
                            std::string id,colorHexa;
                            for (auto* f : e->getChildByName("colors")->getChildIterator()) {                              
                                for (auto* g : f->getChildIterator()) {
                                    const juce::String toto=g->getAllSubText();
                                    if (g->getTagName() == "id") {id=toto.toStdString();}
                                    if (g->getTagName() == "color") {colorHexa=toto.toStdString();}
                                }
                                combo_1_colors[i].init(std::stoi(id,nullptr,16),juce::Colour(std::stoul(colorHexa,nullptr,16)) );
                                i++;
                            }
                        }
                        if (e->getTagName() == "textbox1") {
                            int i=0;
                            std::string id,colorHexa;
                            for (auto* f : e->getChildByName("colors")->getChildIterator()) {                              
                                for (auto* g : f->getChildIterator()) {
                                    const juce::String toto=g->getAllSubText();
                                    if (g->getTagName() == "id") {id=toto.toStdString();}
                                    if (g->getTagName() == "color") {colorHexa=toto.toStdString();}
                                }
                                textbox_1_colors[i].init(std::stoi(id,nullptr,16),juce::Colour(std::stoul(colorHexa,nullptr,16)) );
                                i++;
                            }
                        }
                        if (e->getTagName() == "label1") {
                            int i=0;
                            std::string id,colorHexa;
                            for (auto* f : e->getChildByName("colors")->getChildIterator()) {                              
                                for (auto* g : f->getChildIterator()) {
                                    const juce::String toto=g->getAllSubText();
                                    if (g->getTagName() == "id") {id=toto.toStdString();}
                                    if (g->getTagName() == "color") {colorHexa=toto.toStdString();}
                                }
                                label_1_colors[i].init(std::stoi(id,nullptr,16),juce::Colour(std::stoul(colorHexa,nullptr,16)) );
                                i++;
                            }
                        }
                        if (e->getTagName() == "imagebutton1") {
                            int i=0;
                            std::string id,colorHexa;
                            for (auto* f : e->getChildByName("colors")->getChildIterator()) {                              
                                for (auto* g : f->getChildIterator()) {
                                    const juce::String toto=g->getAllSubText();
                                    if (g->getTagName() == "id") {id=toto.toStdString();}
                                    if (g->getTagName() == "color") {colorHexa=toto.toStdString();}
                                }
                                imagebutton_1_colors[i].init(std::stoi(id,nullptr,16),juce::Colour(std::stoul(colorHexa,nullptr,16)) );
                                i++;
                            }
                        }
                        
                    }  
			} 
        }
    }


    initDirectories();

    //IMAGES
    stepseq_off= juce::ImageFileFormat::loadFrom(juce::File(imagePath+"/stepseq_off.png"));
    stepseq_on=juce::ImageFileFormat::loadFrom(juce::File(imagePath+"/stepseq_on.png"));
    plugin_banner=juce::ImageFileFormat::loadFrom(juce::File(imagePath+"/banner_4.png"));

    // IMAGEBUTTON 1 template
    for(int i = 0; i < 16;i++) {
        seq_step[i].setImages (false, true, true,stepseq_off, 1.000f, imagebutton_1_colors[0].colorValue,juce::Image(), 1.000f, imagebutton_1_colors[1].colorValue,stepseq_on, 1.000f, imagebutton_1_colors[0].colorValue);
    } 
    seq_clear.setImages (false, true, true,stepseq_off, 1.000f, imagebutton_1_colors[1].colorValue,juce::Image(), 1.000f, imagebutton_1_colors[0].colorValue,stepseq_on, 1.000f, imagebutton_1_colors[0].colorValue);
    seq_reset.setImages (false, true, true,stepseq_off, 1.000f, imagebutton_1_colors[1].colorValue,juce::Image(), 1.000f, imagebutton_1_colors[0].colorValue,stepseq_on, 1.000f, imagebutton_1_colors[0].colorValue);
    seq_copy.setImages (false, true, true,stepseq_off, 1.000f, imagebutton_1_colors[1].colorValue,juce::Image(), 1.000f, imagebutton_1_colors[0].colorValue,stepseq_on, 1.000f, imagebutton_1_colors[0].colorValue);
    seq_paste.setImages (false, true, true,stepseq_off, 1.000f, imagebutton_1_colors[1].colorValue,juce::Image(), 1.000f, imagebutton_1_colors[0].colorValue,stepseq_on, 1.000f, imagebutton_1_colors[0].colorValue);
    seq_auto.setImages (false, true, true,stepseq_off, 1.000f, imagebutton_1_colors[1].colorValue,juce::Image(), 1.000f, imagebutton_1_colors[1].colorValue,stepseq_on, 1.000f, imagebutton_1_colors[0].colorValue);
    banner.setImages (false, true, true,plugin_banner, 1.000f,imagebutton_1_colors[1].colorValue,juce::Image(), 1.000f, imagebutton_1_colors[0].colorValue,juce::Image(), 1.000f, imagebutton_1_colors[0].colorValue);
    stepseq_color=(long)imagebutton_1_colors[0].colorValue.getARGB();

    // SLIDER 1 template
    for(int i=0;i<sizeof(slider_1_colors)/sizeof(slider_1_colors[0]);i++) {
        gain__slider->setColour(slider_1_colors[i].colorId,slider_1_colors[i].colorValue);
        mix__slider->setColour(slider_1_colors[i].colorId,slider_1_colors[i].colorValue);
        seq_gate_length.setColour(slider_1_colors[i].colorId,slider_1_colors[i].colorValue);
        seq_length.setColour(slider_1_colors[i].colorId,slider_1_colors[i].colorValue);
    }
    
    // COMBO 1 TEMPLATE
    for(int i=0;i<sizeof(combo_1_colors)/sizeof(combo_1_colors[0]);i++) {
        seq_pattern_selected.setColour(combo_1_colors[i].colorId,combo_1_colors[i].colorValue);
        seq_sequence_selected.setColour(combo_1_colors[i].colorId,combo_1_colors[i].colorValue);
        seq_clock.setColour(combo_1_colors[i].colorId,combo_1_colors[i].colorValue);
        seq_mode.setColour(combo_1_colors[i].colorId,combo_1_colors[i].colorValue);
        seq_env.setColour(combo_1_colors[i].colorId,combo_1_colors[i].colorValue);

        for(int j = 0; j < 16;j++) {
            sseq_step[j].setColour(combo_1_colors[i].colorId,combo_1_colors[i].colorValue);
        }
    }    

    // LABEL 1 TEMPLATE
    for(int i=0;i<sizeof(label_1_colors)/sizeof(label_1_colors[0]);i++) {
        mix_gain_label.setColour(label_1_colors[i].colorId,label_1_colors[i].colorValue);
        mix_amnt_label.setColour(label_1_colors[i].colorId,label_1_colors[i].colorValue);
        seq_clock_label.setColour(label_1_colors[i].colorId,label_1_colors[i].colorValue);
        seq_mode_label.setColour(label_1_colors[i].colorId,label_1_colors[i].colorValue);
        seq_env_label.setColour(label_1_colors[i].colorId,label_1_colors[i].colorValue);
        seq_gate_length_label.setColour(label_1_colors[i].colorId,label_1_colors[i].colorValue);
        seq_auto_label.setColour(label_1_colors[i].colorId,label_1_colors[i].colorValue);
        seq_copy_label.setColour(label_1_colors[i].colorId,label_1_colors[i].colorValue);
        seq_paste_label.setColour(label_1_colors[i].colorId,label_1_colors[i].colorValue);
        seq_reset_label.setColour(label_1_colors[i].colorId,label_1_colors[i].colorValue);
        seq_clear_label.setColour(label_1_colors[i].colorId,label_1_colors[i].colorValue);
        seq_length_label.setColour(label_1_colors[i].colorId,label_1_colors[i].colorValue);

        for(int j = 0; j < sizeof(seq_step_l)/sizeof(seq_step_l[0]);j	++) {
            seq_step_l[j].setColour(label_1_colors[i].colorId,label_1_colors[i].colorValue);
        }
    }
    // TEXTBOX 1 TEMPLATE
    for(int i=0;i<sizeof(textbox_1_colors)/sizeof(textbox_1_colors[0]);i++) {
        debug.setColour(textbox_1_colors[i].colorId,textbox_1_colors[i].colorValue);
        //debug.applyColourToAllText(textbox_1_colors[i].colorValue);
    }

    repaint();

}   

void ChopperAudioProcessorEditor::initDirectories()
{
    juce::File homeDir = juce::File(juce::File::getSpecialLocation (juce::File::userHomeDirectory));
    const std::string homePath=homeDir.getFullPathName().toStdString();
    
    std::string skinsPath,defaultSkinPath,rootPath;
    
   //SystemStats::getOperatingSystemType()
    rootPath=homePath + "/.ssabug";
    dataPath=rootPath + "/choppah";
    configPath=dataPath+"/config";
    presetPath=dataPath+"/presets";
    skinsPath=dataPath+"/skins";
    defaultSkinPath=skinsPath+"/"+currentSkin;
    imagePath=dataPath+ "/skins/" + currentSkin + "/images/";

    if ( not std::filesystem::exists(std::string(rootPath)) )  {
        std::filesystem::create_directory(rootPath);
    }

    if ( not std::filesystem::exists(std::string(dataPath)) )  {
        std::filesystem::create_directory(dataPath);
        std::filesystem::create_directory(configPath);
        std::filesystem::create_directory(presetPath);
        std::filesystem::create_directory(skinsPath);

        std::filesystem::create_directory(defaultSkinPath);
        std::filesystem::create_directory(defaultSkinPath+ "/images");
    }
   
}

void ChopperAudioProcessorEditor::LoadXMLConfig(bool reloadAll=false)
{
    
    std::string xmlFilePath=configPath+"/config.xml";
    if ( not std::filesystem::exists(std::string(xmlFilePath)) )  {

    } else {
        juce::File xmlFile(xmlFilePath);
        juce::XmlDocument xmlDoc(xmlFile);

        if (juce::XmlDocument::parse(xmlFile))
        {
            // Access the root element of the XML document
            auto rootElement = xmlDoc.getDocumentElement();
            if (rootElement->hasTagName("choppah")) {
                for (auto* e : rootElement->getChildByName("options")->getChildIterator())
                    {
                        //debug.setText(debug.getText()+"\n" +e->getTagName()  + " : " +  e->getAllSubText());
                        juce::String paramName = e->getAllSubText();
                                               
                        if (e->getTagName() == "skin") { currentSkin=paramName.toStdString();}
                    } 
                if (reloadAll) {
                    int patternNum=0; 
                    for (auto* e : rootElement->getChildByName("patterns")->getChildIterator())
                        {
                            //debug.setText(debug.getText()+"\n" +e->getTagName()  + " : " +  e->getAllSubText());
                            std::string paramName = e->getTagName().toStdString();
                            std::string paramValue = e->getAllSubText().toStdString();
                            
                            if (paramName.find("pattern") != std::string::npos) {
                                 for (int i=0;i<16;i++) {
                                    patterns[patternNum][15-i]=( stoi(paramValue) >> i) & 1;
                                 }
                            patternNum++;
                            } 
                            
                        } 
                    int sequenceNum=0;
                    for (auto* e : rootElement->getChildByName("sequences")->getChildIterator())
                        {
                            std::string paramName = e->getTagName().toStdString();
                            std::string paramValue = e->getAllSubText().toStdString();
                            
                            if (paramName.find("sequence") != std::string::npos) {
                                 for (int i=0;i<16;i++) {
                                    int bit=i*4;
                                    sequences[sequenceNum][15-i]=(int)( ((stoul(paramValue) >> bit) & 1)*1+ ((stoul(paramValue) >> bit+1) & 1)*2  +((stoul(paramValue) >> bit+2) & 1) *4 + ((stoul(paramValue) >> bit+3) & 1)*8 );
                                 }
                            sequenceNum++;
                            } 
                        }
                   }
			} 
        }
    }
    if (reloadAll) {
        init_all_sequences(0,seq_sequence_selected.getSelectedItemIndex()+1);
    } // update all patterns & sequencesof processor
   // if (currentSkin == "" ) {currentSkin="default";}
   /* debug.setText(debug.getText() + "\n" + 
                   "imagePath " + imagePath+ "\n" +  
                    "dataPath " + dataPath+ "\n" + 
                    "configPath " + configPath+ "\n" + 
                    "presetPath " + presetPath
                    );*/
}


