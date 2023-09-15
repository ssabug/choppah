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
    for (int j=0;j<16;j++) {
        //std::string bitarray;
        //debug.setText (debug.getText() + "\n" + std::__cxx11::to_string(audioProcessor.sequences[j]), juce::dontSendNotification);
        for (int i=0;i<16;i++) {patterns[j][i]=(audioProcessor.sequences[j] >> i)&1 ;sequences[j][i]=audioProcessor.pattern_seqs[j][i];}
        //ui_debug( std::__cxx11::to_string( bitArrayToInt32(patterns[j],16) ) );
    }

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1060, 250);

    // VARIABLES THAT MAY BE SHARED WITH PROCESSOR
    int seq_size=16;
    int seq_pattern_count=16;
    int seq_sequence_count=16;

    // POSITIONS
    int bottomy=130;
    int pseq_posx=100,pseq_posy=20; 
    int sseq_posx=100,sseq_posy=70;   
    int pseq_params_posx=pseq_posx-80, pseq_params_posy=pseq_posy+bottomy;
    int mix_posx=840, mix_posy=pseq_posy+bottomy;
    int mix_label_posx=mix_posx+30, mix_label_posy=mix_posy-20;
    int util_posx=500, util_posy=pseq_posy+bottomy;
    int debug_posx=util_posx-210+10, debug_posy=pseq_posy+bottomy;
    int banner_posx=640, banner_posy=bottomy-50;
    //COLOURS
    juce::Colour color_text2=juce::Colours::blue;
    juce::Colour color_text1=juce::Colours::blue;
    juce::Colour control_label_color=juce::Colour (0, 0, 200);
    juce::Colour control_value_color=juce::Colour (0, 0, 255);
    juce::Colour background_color=juce::Colours::black;
    int color_stepseq_1=0xFF00007F;
    int color_stepseq_2=0xFF0000FF;
    //IMAGES
    stepseq_off= juce::ImageFileFormat::loadFrom(juce::File(imagePath+"stepseq_off.png"));
    stepseq_on=juce::ImageFileFormat::loadFrom(juce::File(imagePath+"stepseq_on.png"));
    plugin_banner=juce::ImageFileFormat::loadFrom(juce::File(imagePath+"banner_4.png"));

    //////////////////////////////// OUT SETTINGS///////////////////////////////
    //NEW IMPL 
    controlColorTemplate slider_1_colors[5];
    slider_1_colors[0].init(0x1001300,juce::Colour(0xFF0000FF));
    slider_1_colors[1].init(0x1001312,juce::Colour(0xFF000000));
    slider_1_colors[2].init(0x1001311,juce::Colour(0xFF0000FF));
    slider_1_colors[3].init(0x1001400,juce::Colour(0xFF0000FF));
    slider_1_colors[4].init(0x1001700,juce::Colour(0xFF000000));   
    controlColorTemplate label_1_colors[1];
    label_1_colors[0].init(0x1000281,juce::Colour(0xFF0000FF));
    controlColorTemplate combo_1_colors[5];
    combo_1_colors[1].init(0x1000b00,juce::Colour(0xFF000000));
    combo_1_colors[1].init(0x1000c00,juce::Colour(0xFF0000FF));
    combo_1_colors[1].init(0x1000e00,juce::Colour(0xFF0000FF));
    combo_1_colors[1].init(0x1000a00,juce::Colour(0xFF0000FF));
    controlColorTemplate stepseq_colors[2];
    stepseq_colors[0].init(0x1000281,juce::Colour(0xFF0000FF));
    stepseq_colors[1].init(0x1000281,juce::Colour(0xFF00007F));
    controlColorTemplate textbox_colors[4];
    textbox_colors[0].init(0x1000201,juce::Colour(0xFF0000FF));
    textbox_colors[1].init(0x1000200,juce::Colour(0xFF000000));
    textbox_colors[2].init(0x1000205,juce::Colour(0xFF000000));
    textbox_colors[3].init(0x1000207,juce::Colour(0xFF000000));
    
    initSlider1("out_gain",gain__slider,slider_1_colors,sizeof(slider_1_colors)/sizeof(slider_1_colors[0]),mix_posx,mix_posy,118,83,0.0f,10.0f,0.01f);            //OUT GAIN
    gain__slider->onValueChange = [this] { out_gain_change(); };
    gain__slider->setValue (audioProcessor.out_gain->get());
    initLabel1(mix_gain_label,"Out Gain",label_1_colors,sizeof(label_1_colors)/sizeof(label_1_colors[0]),mix_label_posx,mix_label_posy,150,24);
 
    initSlider1("out_mix",mix__slider,slider_1_colors,sizeof(slider_1_colors)/sizeof(slider_1_colors[0]),mix_posx+100, mix_posy, 118, 83,0.0,1.0,0.01);       // DRY/WET
    mix__slider->onValueChange = [this] { out_mix_change(); };
    mix__slider->setValue (audioProcessor.out_mix->get());
    initLabel1(mix_amnt_label,"Dry/Wet",label_1_colors,sizeof(label_1_colors)/sizeof(label_1_colors[0]),mix_label_posx+100,mix_label_posy,150,24);
    
    initCombo1(seq_pattern_selected,combo_1_colors,sizeof(combo_1_colors)/sizeof(combo_1_colors[0]),pseq_posx-85,pseq_posy-3 , 60, 24);          // PATTERN SELECT
    for(int i = 1; i <= seq_pattern_count; i++) { seq_pattern_selected.addItem (TRANS(std::to_string(i)), i);   } 
    seq_pattern_selected.onChange = [this] { step_seq_change(); };
    seq_pattern_selected.setSelectedItemIndex(audioProcessor.pseq_current->get()-1);
    
    initCombo1(seq_sequence_selected,combo_1_colors,sizeof(combo_1_colors)/sizeof(combo_1_colors[0]),sseq_posx-85, sseq_posy-5, 60, 24);         // SEQUENCE SELECT
    for(int i = 1; i <= seq_pattern_count; i++) { seq_sequence_selected.addItem (TRANS(std::to_string(i)), i);   }
    seq_sequence_selected.onChange = [this] { seq_pattern_change(); };
    seq_sequence_selected.setSelectedItemIndex(audioProcessor.sseq_current->get()-1);

    initCombo1(seq_clock,combo_1_colors,sizeof(combo_1_colors)/sizeof(combo_1_colors[0]),pseq_params_posx, pseq_params_posy, 86, 24);                     // CLOCK SELECT
    initLabel1(seq_clock_label,"Clock div",label_1_colors,sizeof(label_1_colors)/sizeof(label_1_colors[0]),pseq_params_posx, pseq_params_posy-20, 86, 24);
    seq_clock.onChange = [this] { seq_clock_change(); };
    seq_clock.addItem (TRANS("4"), 1);seq_clock.addItem (TRANS("2"), 2);seq_clock.addItem (TRANS("1"), 3);seq_clock.addItem (TRANS("1/2"), 4);
    seq_clock.addItem (TRANS("1/4"), 5);seq_clock.addItem (TRANS("1/8"), 6);seq_clock.addItem (TRANS("1/16"), 7);seq_clock.addItem (TRANS("1/32"), 8);
    seq_clock.setSelectedItemIndex(audioProcessor.clock_div->getIndex());

    initCombo1(seq_mode,combo_1_colors,sizeof(combo_1_colors)/sizeof(combo_1_colors[0]),pseq_params_posx,pseq_params_posy+50,86,24);                     // MODE SELECT
    initLabel1(seq_mode_label,"Mode",label_1_colors,sizeof(label_1_colors)/sizeof(label_1_colors[0]),pseq_params_posx+20,pseq_params_posy-20+50,150,24);
    seq_mode.onChange = [this] { seq_mode_change(); };
    seq_mode.addItem (TRANS("pattern"), 1);seq_mode.addItem (TRANS("sequence"), 2);
    seq_mode.setSelectedItemIndex(audioProcessor.seq_mode->getIndex());

    initCombo1(seq_env,combo_1_colors,sizeof(combo_1_colors)/sizeof(combo_1_colors[0]),pseq_params_posx+100, pseq_params_posy, 86, 24);                     // ENV SELECT
    initLabel1(seq_env_label,"Env",label_1_colors,sizeof(label_1_colors)/sizeof(label_1_colors[0]),pseq_params_posx+100, pseq_params_posy-20, 86, 24);
    seq_env.onChange = [this] { seq_env_change(); };
    seq_env.addItem (TRANS("flat"), 1);seq_env.addItem (TRANS("sharp"), 2);seq_env.addItem (TRANS("tri"), 3);
    seq_env.setSelectedItemIndex(audioProcessor.seq_env->getIndex());

    initSlider2(seq_gate_length,slider_1_colors,sizeof(slider_1_colors)/sizeof(slider_1_colors[0]),pseq_params_posx+80, pseq_params_posy, 118, 83,0, 100, 1);  // GATE LENGTH
    initLabel1(seq_gate_length_label,"Gate length",label_1_colors,sizeof(label_1_colors)/sizeof(label_1_colors[0]),pseq_params_posx+80+20, pseq_params_posy-20, 86, 24);
    seq_gate_length.onValueChange = [this] { seq_gate_length_change(); };
    seq_gate_length.setValue (100); 
   
    initSlider2(seq_length,slider_1_colors,sizeof(slider_1_colors)/sizeof(slider_1_colors[0]),pseq_params_posx+100, pseq_params_posy+50, 86, 28,2, 16, 1);  // SEQUENCE LENGTH
    initLabel1(seq_length_label,"Seq length",label_1_colors,sizeof(label_1_colors)/sizeof(label_1_colors[0]),pseq_params_posx+100, pseq_params_posy-20+50, 86, 24);
    seq_length.onValueChange = [this] { seq_length_change(); };
    seq_length.setValue (audioProcessor.sseq_length->get());  
    
    for(int i = 0; i < sizeof(seq_step_l)/sizeof(seq_step_l[0]); i++) {                             // 4 STEP MARKS
            seq_step_l[i].setText ("______", juce::dontSendNotification);
            seq_step_l[i].setColour(juce::Label::textColourId,color_text2);
            seq_step_l[i].setBounds (pseq_posx+i*240, pseq_posy+10, 46, 16);
            addAndMakeVisible (seq_step_l[i]);
        }
       
    for(int i = 0; i < seq_size;i++) {
        const int idx=i;
        initStepSeqButton(seq_step[i],idx,stepseq_colors,sizeof(stepseq_colors)/sizeof(stepseq_colors[0]),pseq_posx+i*60, pseq_posy, 46, 16);  // PATTERN BUTTONS 
    }
    seq_step[0].onClick = [this] { step_seq_click(0); };seq_step[8].onClick = [this] { step_seq_click(8); };seq_step[4].onClick = [this] { step_seq_click(4); };seq_step[12].onClick = [this] { step_seq_click(12); };
    seq_step[1].onClick = [this] { step_seq_click(1); };seq_step[9].onClick = [this] { step_seq_click(9); };seq_step[5].onClick = [this] { step_seq_click(5); };seq_step[13].onClick = [this] { step_seq_click(13); };
    seq_step[2].onClick = [this] { step_seq_click(2); };seq_step[10].onClick = [this] { step_seq_click(10); };seq_step[6].onClick = [this] { step_seq_click(6); };seq_step[14].onClick = [this] { step_seq_click(14); };
    seq_step[3].onClick = [this] { step_seq_click(3); };seq_step[11].onClick = [this] { step_seq_click(11); };seq_step[7].onClick = [this] { step_seq_click(7); };seq_step[15].onClick = [this] { step_seq_click(15); };
    
    initUtilityButton(seq_clear,stepseq_colors,sizeof(stepseq_colors)/sizeof(stepseq_colors[0]),util_posx, util_posy, 46, 16);              // CLEAR
    seq_clear.onClick = [this] { step_seq_clear();};
    initLabel1(seq_clear_label,"clear",label_1_colors,sizeof(label_1_colors)/sizeof(label_1_colors[0]),util_posx, util_posy-20, 86, 24);    
    
    initUtilityButton(seq_reset,stepseq_colors,sizeof(stepseq_colors)/sizeof(stepseq_colors[0]),util_posx, util_posy+40, 46, 16);              // RESET
    seq_reset.onClick = [this] { step_seq_reset();};
    initLabel1(seq_reset_label,"reset",label_1_colors,sizeof(label_1_colors)/sizeof(label_1_colors[0]),util_posx, util_posy+40-20, 86, 24);

    initUtilityButton(seq_copy,stepseq_colors,sizeof(stepseq_colors)/sizeof(stepseq_colors[0]),util_posx+60, util_posy, 46, 16);              // COPY
    seq_copy.onClick = [this] { step_seq_copy();};
    initLabel1(seq_copy_label,"copy",label_1_colors,sizeof(label_1_colors)/sizeof(label_1_colors[0]),util_posx+60, util_posy-20, 86, 24);
    
    initUtilityButton(seq_paste,stepseq_colors,sizeof(stepseq_colors)/sizeof(stepseq_colors[0]),util_posx+60, util_posy+40, 46, 16);              // PASTE
    seq_paste.onClick = [this] { step_seq_paste();};
    initLabel1(seq_paste_label,"paste",label_1_colors,sizeof(label_1_colors)/sizeof(label_1_colors[0]),util_posx+60, util_posy+40-20, 86, 24);

    initUtilityButton(seq_auto,stepseq_colors,sizeof(stepseq_colors)/sizeof(stepseq_colors[0]),pseq_params_posx+200, pseq_params_posy, 46, 16);              // AUTO
    seq_auto.onClick = [this] { seq_auto_click();};
    initLabel1(seq_auto_label,"Auto",label_1_colors,sizeof(label_1_colors)/sizeof(label_1_colors[0]),pseq_params_posx+200, pseq_params_posy-20, 86, 24);

    for(int i = 0; i < seq_size; i++) {
        initCombo1(sseq_step[i],combo_1_colors,sizeof(combo_1_colors)/sizeof(combo_1_colors[0]),sseq_posx-3+i*60, sseq_posy, 53, 16);                   // SEQUENCE SEQ COMBOBOXES
        sseq_step[i].addItem (TRANS("1"), 1);sseq_step[i].addItem (TRANS("2"), 2);sseq_step[i].addItem (TRANS("3"), 3);sseq_step[i].addItem (TRANS("4"), 4);
        sseq_step[i].addItem (TRANS("5"), 5);sseq_step[i].addItem (TRANS("6"), 6);sseq_step[i].addItem (TRANS("7"), 7);sseq_step[i].addItem (TRANS("8"), 8);
        sseq_step[i].addItem (TRANS("9"), 9);sseq_step[i].addItem (TRANS("10"), 10);sseq_step[i].addItem (TRANS("11"), 11);sseq_step[i].addItem (TRANS("12"), 12);
        sseq_step[i].addItem (TRANS("13"), 13);sseq_step[i].addItem (TRANS("14"), 14);sseq_step[i].addItem (TRANS("15"), 15);sseq_step[i].addItem (TRANS("16"), 16);
        sseq_step[i].setSelectedItemIndex(0);
        addAndMakeVisible (sseq_step[i]);
    }
    sseq_step[0].onChange = [this] { seq_seq_change(0); };sseq_step[4].onChange = [this] { seq_seq_change(4); };sseq_step[8].onChange = [this] { seq_seq_change(8); };sseq_step[12].onChange = [this] { seq_seq_change(12); };
    sseq_step[1].onChange = [this] { seq_seq_change(1); };sseq_step[5].onChange = [this] { seq_seq_change(5); };sseq_step[9].onChange = [this] { seq_seq_change(9); };sseq_step[13].onChange = [this] { seq_seq_change(13); };
    sseq_step[2].onChange = [this] { seq_seq_change(2); };sseq_step[6].onChange = [this] { seq_seq_change(6); };sseq_step[10].onChange = [this] { seq_seq_change(10); };sseq_step[14].onChange = [this] { seq_seq_change(14); };
    sseq_step[3].onChange = [this] { seq_seq_change(3); };sseq_step[7].onChange = [this] { seq_seq_change(7); };sseq_step[11].onChange = [this] { seq_seq_change(11); };sseq_step[15].onChange = [this] { seq_seq_change(15); };    

    /*
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
    gain__slider->setValue (audioProcessor.out_gain->get() );
    mix_gain_label.setText ("Out Gain", juce::dontSendNotification);
    mix_gain_label.setBounds(mix_label_posx,mix_label_posy,150,24);
    mix_gain_label.setColour(juce::Label::textColourId,control_label_color);*/
    /*
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
    mix__slider->setValue (audioProcessor.out_mix->get());
    mix_amnt_label.setText ("Dry/Wet", juce::dontSendNotification);
    mix_amnt_label.setBounds(mix_label_posx+100,mix_label_posy,150,24);
    mix_amnt_label.setColour(juce::Label::textColourId,control_label_color);
    
    //juce::AudioProcessorParameter::getValueForText("out_mix")
    /*auto& param = processor.getParameter(ChopperAudioProcessor::out_mix);
    auto* attachment = new juce::AudioProcessorValueTreeState::SliderAttachment(processor.getParameterTree(), param, mix__slider);*/

    //////////////////////////////PATTERN SEQ//////////////////////////////////////
    /*for(int i = 0; i < seq_size;i++) {
        //seq_step[i].setButtonText (TRANS("stepseq"<<i));
        seq_step[i].setToggleable(true);
        seq_step[i].setImages (false, true, true,stepseq_off, 1.000f, juce::Colour (color_stepseq_1),juce::Image(), 1.000f, juce::Colour (color_stepseq_2),stepseq_on, 1.000f, juce::Colour (color_stepseq_1));
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
    /*
    seq_pattern_selected.setEditableText (false);
    for(int i = 1; i <= seq_pattern_count; i++) { seq_pattern_selected.addItem (TRANS(std::to_string(i)), i);   }
    seq_pattern_selected.setSelectedItemIndex(audioProcessor.pseq_current->get()-1);
    seq_pattern_selected.setColour(juce::Label::textColourId,color_text1);
    seq_pattern_selected.setColour(juce::ComboBox::backgroundColourId,juce::Colours::black);
    seq_pattern_selected.setColour(juce::ComboBox::outlineColourId ,juce::Colours::blue);
    seq_pattern_selected.setColour(juce::ComboBox::arrowColourId ,control_value_color);
    seq_pattern_selected.setColour(juce::ComboBox::textColourId,control_value_color);
    seq_pattern_selected.setBounds (pseq_posx-85,pseq_posy-3 , 60, 24);
    seq_pattern_selected.onChange = [this] { step_seq_change(); };*/
    

    //////////////////////////////SEQUENCE SEQ////////////////////////////////////// 
    /*for(int i = 0; i < seq_size; i++) {
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
    seq_sequence_selected.setSelectedItemIndex(audioProcessor.sseq_current->get()-1);
    seq_sequence_selected.setBounds (sseq_posx-85, sseq_posy-5, 60, 24);

    
    ////////////////////////////// SEQ PARAMS ////////////////////////////////////// 
    seq_clock.setEditableText (false);
    seq_clock.setJustificationType (juce::Justification::centredLeft);
    seq_clock.onChange = [this] { seq_clock_change(); };
    seq_clock.addItem (TRANS("4"), 1);seq_clock.addItem (TRANS("2"), 2);seq_clock.addItem (TRANS("1"), 3);seq_clock.addItem (TRANS("1/2"), 4);
    seq_clock.addItem (TRANS("1/4"), 5);seq_clock.addItem (TRANS("1/8"), 6);seq_clock.addItem (TRANS("1/16"), 7);seq_clock.addItem (TRANS("1/32"), 8);
    seq_clock.setSelectedItemIndex(audioProcessor.clock_div->getIndex());
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
    seq_gate_length.setValue (audioProcessor.pseq_gate_length->get());
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
    seq_mode.setSelectedItemIndex(audioProcessor.seq_mode->getIndex());
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
    seq_env.addItem (TRANS("saw"), 3);
    seq_env.addItem (TRANS("ramp"), 4);
    seq_env.addItem (TRANS("spike"), 5);
    seq_env.addItem (TRANS("slow"), 6);
    seq_env.setSelectedItemIndex(audioProcessor.seq_env->getIndex());
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
    seq_length.setValue(audioProcessor.sseq_length->get());
    seq_length_label.setText ("Seq length", juce::dontSendNotification);
    seq_length_label.setColour(juce::Label::textColourId,control_label_color);
    seq_length_label.setBounds (pseq_params_posx+100, pseq_params_posy-20+50, 86, 24);*/
    /*
    seq_auto.setToggleable(true);
    seq_auto.setImages (false, true, true,
                              stepseq_off, 1.000f, juce::Colour (color_stepseq_1),
                                  juce::Image(), 1.000f, juce::Colour (color_stepseq_2),
                                  stepseq_on, 1.000f, juce::Colour (color_stepseq_1));
    seq_auto.setBounds (pseq_params_posx+200, pseq_params_posy, 46, 16);
    seq_auto.onClick = [this] { seq_auto_click();};
    seq_auto_label.setColour(juce::Label::textColourId,control_label_color);
    seq_auto_label.setText ("Auto", juce::dontSendNotification);
    seq_auto_label.setBounds (pseq_params_posx+200, pseq_params_posy-20, 86, 24);

    ////////////////////////////// UTILITY ////////////////////////////////////// 
    seq_clear.setToggleable(true);
    seq_clear.setImages (false, true, true,stepseq_off, 1.000f, juce::Colour (color_stepseq_1),juce::Image(), 1.000f, juce::Colour (color_stepseq_2),stepseq_on, 1.000f, juce::Colour (color_stepseq_1));
    seq_clear.setBounds (util_posx, util_posy, 46, 16);
    seq_clear.onClick = [this] { step_seq_clear();};
    seq_clear_label.setColour(juce::Label::textColourId,control_label_color);
    seq_clear_label.setText ("clear", juce::dontSendNotification);
    seq_clear_label.setBounds (util_posx, util_posy-20, 86, 24);

    seq_reset.setToggleable(true);
    seq_reset.setImages (false, true, true,stepseq_off, 1.000f, juce::Colour (color_stepseq_1),juce::Image(), 1.000f, juce::Colour (color_stepseq_2),stepseq_on, 1.000f, juce::Colour (color_stepseq_1));
    seq_reset.setBounds (util_posx, util_posy+40, 46, 16);
    seq_reset.onClick = [this] { step_seq_reset();};
    seq_reset_label.setColour(juce::Label::textColourId,control_label_color);
    seq_reset_label.setText ("reset", juce::dontSendNotification);
    seq_reset_label.setBounds (util_posx, util_posy+40-20, 86, 24);

    seq_copy.setToggleable(true);
    seq_copy.setImages (false, true, true,stepseq_off, 1.000f, juce::Colour (color_stepseq_1),juce::Image(), 1.000f, juce::Colour (color_stepseq_2),stepseq_on, 1.000f, juce::Colour (color_stepseq_1));
    seq_copy.setBounds (util_posx+60, util_posy, 46, 16);
    seq_copy.onClick = [this] { step_seq_copy();};
    seq_copy_label.setColour(juce::Label::textColourId,control_label_color);
    seq_copy_label.setText ("copy", juce::dontSendNotification);
    seq_copy_label.setBounds (util_posx+60, util_posy-20, 86, 24);

    seq_paste.setToggleable(true);
    seq_paste.setImages (false, true, true,stepseq_off, 1.000f, juce::Colour (color_stepseq_1),juce::Image(), 1.000f, juce::Colour (color_stepseq_2),stepseq_on, 1.000f, juce::Colour (color_stepseq_1));
    seq_paste.setBounds (util_posx+60, util_posy+40, 46, 16);
    seq_paste.onClick = [this] { step_seq_paste();};
    seq_paste_label.setColour(juce::Label::textColourId,control_label_color);
    seq_paste_label.setText ("paste", juce::dontSendNotification);
    seq_paste_label.setBounds (util_posx+60, util_posy+40-20, 86, 24);*/
    /////////////////////////////////////BANNER///////////////////////////////////////
    banner.setToggleable(true);
    banner.setImages (false, true, true,plugin_banner, 1.000f,stepseq_colors[1].colorValue,juce::Image(), 1.000f, stepseq_colors[0].colorValue,juce::Image(), 1.000f, stepseq_colors[1].colorValue);
    banner.setBounds (banner_posx,banner_posy+30, 190, 120);
    banner.onClick = [this] { switch_skins();};

    //////////////////////////////////////DEBUG///////////////////////////////////////
    debug.setColour(label_1_colors[0].colorId,label_1_colors[0].colorValue);
    //debug.setText ("debug", juce::dontSendNotification);
    debug.setBounds (debug_posx, debug_posy-20, 170, 100);
    for (auto c : textbox_colors) { debug.setColour(c.colorId,juce::Colour(c.colorValue));  }
    debugB.setBounds (debug_posx-20-60, debug_posy-20+70, 15, 15);
    debugB.onClick = [this] { debugF(true);};
    debugBB.setBounds (debug_posx-20-30, debug_posy-20+70, 15, 15);
    debugBB.onClick = [this] { writeXMLConfig(true,true,true);};

    //init_all_sequences(0,0);

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
    addAndMakeVisible (debugBB);

    LoadXMLConfig(true,false,false);
    reloadSkinFromXML();
    
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
////////////////////////////////////////////////////////////////////////////////// TEMPLATES ////////////////////////////////////////////////////////////////////////
void ChopperAudioProcessorEditor::initSlider1(std::string name,std::unique_ptr<juce::Slider>& slider,controlColorTemplate slider_colors[],int colorCount, int x,int y,int w,int h,float min,float max,float def)
{   slider.reset (new juce::Slider (name));   
    slider->setSliderStyle (juce::Slider::Rotary);
    slider->setRange (min, max, def);
    slider->setTextBoxStyle (juce::Slider::TextBoxBelow, true, 80, 20);
    slider->setBounds (x, y, w, h);
    for(int i=0;i<colorCount;i++) {
        slider->setColour(slider_colors[i].colorId,slider_colors[i].colorValue);
    }
}

void ChopperAudioProcessorEditor::initSlider2(juce::Slider& slider,controlColorTemplate slider_colors[],int colorCount,int x,int y,int w,int h,float min,float max,float def)
{   slider.setSliderStyle (juce::Slider::LinearHorizontal );
    slider.setRange (min, max, def);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, true, 80, 20);
    slider.setBounds (x, y, w, h);        
    for(int i=0;i<colorCount;i++) {
        slider.setColour(slider_colors[i].colorId,slider_colors[i].colorValue);
    }
}

void ChopperAudioProcessorEditor::initCombo1(juce::ComboBox& comboBox,controlColorTemplate combo_colors[],int colorCount, int x,int y,int w,int h)
{   comboBox.setEditableText (false);
    comboBox.setJustificationType (juce::Justification::centredLeft);
    for(int i=0;i<colorCount;i++) {
        comboBox.setColour(combo_colors[i].colorId,combo_colors[i].colorValue);
    }
    comboBox.setBounds (x, y, w, h);    
}

void ChopperAudioProcessorEditor::initLabel1(juce::Label& label,std::string text,controlColorTemplate label_colors[], int colorCount, int x,int y,int w,int h)
{   label.setText (text, juce::dontSendNotification);
    for(int i=0;i<colorCount;i++) {
        label.setColour(label_colors[i].colorId,label_colors[i].colorValue);
    }
    label.setBounds (x, y, w, h);
}

void ChopperAudioProcessorEditor::initStepSeqButton(juce::ImageButton& button,const int index,controlColorTemplate imagebutton_colors[],int colorCount, int x,int y,int w,int h)
{
    juce::Colour color_stepseq_1=imagebutton_colors[1].colorValue;
    juce::Colour color_stepseq_2=imagebutton_colors[0].colorValue;
    button.setToggleable(true);
    button.setImages (false, true, true,stepseq_off, 1.000f, color_stepseq_1,juce::Image(), 1.000f, color_stepseq_2,stepseq_on, 1.000f, color_stepseq_1);
    button.setBounds (x, y, w, h);
    addAndMakeVisible (button);
    //button.onClick = [this] { step_seq_click(index);
    //sequence[i]=false;
}
    
void ChopperAudioProcessorEditor::initUtilityButton(juce::ImageButton& button,controlColorTemplate imagebutton_colors[],int colorCount, int x,int y,int w,int h)
{
    juce::Colour color_stepseq_1=imagebutton_colors[1].colorValue;
    juce::Colour color_stepseq_2=imagebutton_colors[0].colorValue;
    button.setToggleable(true);
    button.setImages (false, true, true,stepseq_off, 1.000f, color_stepseq_1,juce::Image(), 1.000f, color_stepseq_2,stepseq_on, 1.000f, color_stepseq_1);
    button.setBounds (x, y, w, h);
} 

int ChopperAudioProcessorEditor::bitArrayToInt32(bool arrr[], int count)
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
////////////////////////////////////////////////////////////////////////////////// TIMER CALLBACK ////////////////////////////////////////////////////////////////////////
void ChopperAudioProcessorEditor::timerCallback()
{
    juce::Colour step_color_full=juce::Colour (0xFFFFFFFF & stepseq_color);
    juce::Colour step_color_dim=juce::Colour  (0xFF7F7F7F & stepseq_color);
    juce::Colour step_color_dimm=juce::Colour (0xFF3F3F3F & stepseq_color);
    juce::Colour step_color_off=juce::Colour  (0xFF000000 & stepseq_color);

    int clockdiv=seq_clock.getSelectedItemIndex();
    int coeff=pow(2,clockdiv);  
    double ppq=audioProcessor.getDAWppq()/coeff;
    double ppqlastbar=audioProcessor.getDAWppqlastbar()/coeff;
    int processorPattern=audioProcessor.pseq_current->get()-1;
    int seqLength=audioProcessor.sseq_length->get();
    int seqClock=audioProcessor.clock_div->getIndex();
    int seqMode=audioProcessor.seq_mode->getIndex(); 
    int autoPattern=audioProcessor.pseq_auto->get();
    int index=std::floor((ppq-std::floor(ppqlastbar))*16);
    int seqIndex;
    //bool ptrn[16];for (int i=0;i<16;i++) {ptrn[i]=patterns[processorPattern][15-i];} //FOR PATTERN CODE BUILDING
    if (index>16) {index-=16*int(std::floor(index/16));}
    if (ppq>=seqLength) {seqIndex=int(std::floor(ppq)-seqLength*std::floor(ppq/seqLength));} else {seqIndex=std::floor(ppq);}

    debug.setText ("ppq = " + std::__cxx11::to_string(ppq)+"\n"+
                   "pattern\t\t\t" + std::__cxx11::to_string(audioProcessor.pseq_current->get())+
                   " step " +std::__cxx11::to_string(index+1)+"\n" +
                   "sequence\t" +std::__cxx11::to_string(seq_sequence_selected.getSelectedItemIndex()+1)+ 
                   " step " +std::__cxx11::to_string(seqIndex+1) +"\n"
                   //+"patterndata " + std::__cxx11::to_string(bitArrayToInt32(ptrn,16))//FOR PATTERN CODE BUILDING
                  , juce::dontSendNotification);  

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
    
    /*if (seq_clock.getSelectedItemIndex() != seqClock) {
        seq_clock.setSelectedItemIndex(seqClock,juce::dontSendNotification);
    }
    if (seq_mode.getSelectedItemIndex() != seqMode) {
        seq_mode.setSelectedItemIndex(seqMode,juce::dontSendNotification);
    }*/
    if (seq_pattern_selected.getSelectedItemIndex() !=  processorPattern && seqMode == 1) {
        seq_pattern_selected.setSelectedItemIndex(processorPattern);
    }
    
}
////////////////////////////////////////////////////////////////////////////////// CONTROL CALLBACKS ////////////////////////////////////////////////////////////////////////
void ChopperAudioProcessorEditor::step_seq_update(int stepIndex)
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

void ChopperAudioProcessorEditor::step_seq_click(int stepIndex)
{
    if ( sequence[stepIndex] == false) { sequence[stepIndex]=true;} else {sequence[stepIndex]=false;}
    
    step_seq_update(stepIndex);      
}

void ChopperAudioProcessorEditor::step_seq_clear() 
{
    for (int i=0;i<sizeof(sequence)/sizeof(sequence[0]);i++) {
        sequence[i]=false;
        step_seq_update(i);
    }
}

void ChopperAudioProcessorEditor::step_seq_reset() 
{
    for (int j=0;j<sizeof(sequence)/sizeof(sequence[0]);j++) {
        for (int i=0;i<sizeof(sequence)/sizeof(sequence[0]);i++) {
            patterns[j][i]=false;
        }
        sequence[j]=false;
        step_seq_update(j);
    }
    
}

void ChopperAudioProcessorEditor::step_seq_copy() 
{      
    for (int i=0;i<sizeof(sequence)/sizeof(sequence[0]);i++) 
    { 
        sequence_clipboard[i]=sequence[i];
        step_seq_update(i);
    }
}

void ChopperAudioProcessorEditor::step_seq_paste() 
{
    for (int i=0;i<sizeof(sequence)/sizeof(sequence[0]);i++) 
    { 
        sequence[i]=sequence_clipboard[i];
        step_seq_update(i);
    }
}

void ChopperAudioProcessorEditor::step_seq_change() 
{ 
    int seqIndex=seq_pattern_selected.getSelectedItemIndex();
    
    for (int i=0;i<sizeof(sequence)/sizeof(sequence[0]);i++) 
        {
            sequence[i]=patterns[seqIndex][i];
            step_seq_update(i);
        }
    audioProcessor.pseq_current->operator=(seqIndex+1);
} 

void ChopperAudioProcessorEditor::seq_seq_change(int step) 
{   
    int seqIndex=seq_sequence_selected.getSelectedItemIndex();
    sequences[seqIndex][step]=sseq_step[step].getSelectedItemIndex();       
    //ssequence[step]=sequences[seqIndex][step];
    audioProcessor.pattern_seqs[seqIndex][step]=sseq_step[step].getSelectedItemIndex();
    
    //audioProcessor.sseq_current->operator=(seqIndex+1);
}

void ChopperAudioProcessorEditor::seq_pattern_change() 
{   
    int seqIndex=seq_sequence_selected.getSelectedItemIndex();  
    for (int i=0;i<sizeof(sequence)/sizeof(sequence[0]);i++) 
        {
            //ssequence[i]=sequences[seqIndex][i];
            sseq_step[i].setSelectedItemIndex(sequences[seqIndex][i]);
        }
    audioProcessor.sseq_current->operator=(seqIndex+1);
}

void ChopperAudioProcessorEditor::seq_env_change() {   audioProcessor.updateEnv(seq_env.getSelectedItemIndex());      }
void ChopperAudioProcessorEditor::seq_length_change() {    audioProcessor.sseq_length->operator=(seq_length.getValue());}
void ChopperAudioProcessorEditor::out_gain_change(){    audioProcessor.out_gain->operator=(gain__slider->getValue());}
void ChopperAudioProcessorEditor::out_mix_change(){    audioProcessor.out_mix->operator=(mix__slider->getValue());}
void ChopperAudioProcessorEditor::seq_mode_change(){    audioProcessor.seq_mode->operator=(seq_mode.getSelectedItemIndex());}
void ChopperAudioProcessorEditor::seq_clock_change(){   audioProcessor.clock_div->operator=(seq_clock.getSelectedItemIndex());}
void ChopperAudioProcessorEditor::seq_gate_length_change(){    audioProcessor.pseq_gate_length->operator=(seq_gate_length.getValue());}
void ChopperAudioProcessorEditor::ui_debug(std::string text){    debug.setText (debug.getText() + "\n" + text, juce::dontSendNotification);}

void ChopperAudioProcessorEditor::init_all_sequences(int returnToPattern=0,int returnToSeq=0)
{
    for (int i=15;i>=0;i--){
        //audioProcessor.sequences[i]=bitArrayToInt32(patterns[i],16);
        seq_pattern_selected.setSelectedItemIndex(i);
        for (int j;j<16;j++){
            audioProcessor.pattern_seqs[i][j]=patterns[i][j];
        }
        
        audioProcessor.updatePattern(i,bitArrayToInt32(patterns[i],16));
    }
    if (returnToPattern>0) {seq_pattern_selected.setSelectedItemIndex(returnToPattern-1);}
    if (returnToSeq>0) {seq_sequence_selected.setSelectedItemIndex(returnToSeq-1);}
    //audioProcessor.initGatesMap();
}

void ChopperAudioProcessorEditor::seq_auto_click()
{
    int value=0;
    if ( audioProcessor.pseq_auto->get() == 0) {
        value=1;
    } else {
        value=0;
    }
    audioProcessor.pseq_auto->operator=(value);
}

void ChopperAudioProcessorEditor::debugF(bool reloadFromFile=false)
{
    if (reloadFromFile) { LoadXMLConfig(true,true,true); }
    reloadSkinFromXML();
}

void ChopperAudioProcessorEditor::switch_skins()
{
    std::string skinsPath=dataPath+ "/skins"; // get skins folder path    
    auto folders=get_directories(skinsPath);//get available folders in /skins
    std::vector<std::string> skinList;//={"default","red","green","yellow","purple","turquoise"};
    for (auto folder : folders) {      
        if ( std::filesystem::exists(folder+"/skin.xml") ) {            // look for skin file
            std::string folderShort=folder.substr(skinsPath.length()+1); // get skin folder name
            //debug.setText(debug.getText()+"\nfound skin "+folderShort);
            skinList.push_back(folderShort);  // add to skin list
        }        
    }
    int currentSkinIndex=0; 
    for (int i=0;i<skinList.size();i++) {  // find current skin index
        if (skinList[i] == currentSkin) { currentSkinIndex=i;break; }
    }
    if (currentSkinIndex == skinList.size()-1) {currentSkinIndex=0;}  // increment skin index
    else {currentSkinIndex++;}
    
    currentSkin=skinList[currentSkinIndex]; 
  
    reloadSkinFromXML();
    
}

/////////////////////////////////////////////////////////////////////// FILES & DIRECTORIES //////////////////////////////////////////////////////////////////////////////////////////

void ChopperAudioProcessorEditor::initDirectories()
{
    juce::File homeDir = juce::File(juce::File::getSpecialLocation (juce::File::userHomeDirectory));
    const std::string homePath=homeDir.getFullPathName().toStdString();
    
    std::string skinsPath,defaultSkinPath,rootPath;
    
   //SystemStats::getOperatingSystemType()
    rootPath=homePath + "/.ssabug";
    dataPath=rootPath + "/choppah";
    configPath=dataPath+"/config/";
    presetPath=dataPath+"/presets";
    skinsPath=dataPath+"/skins";
    defaultSkinPath=skinsPath+"/"+currentSkin;
    currentSkinPath=defaultSkinPath+"/";
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


std::vector<std::string> ChopperAudioProcessorEditor::get_directories(const std::string& s)
{
    std::vector<std::string> r;
    for(auto& p : std::filesystem::recursive_directory_iterator(s))
        if (p.is_directory())
            r.push_back(p.path().string());
    return r;
}

/////////////////////////////////////////////////////////////////////// XML //////////////////////////////////////////////////////////////////////////////////////////
void ChopperAudioProcessorEditor::reloadSkinFromXML()
{
    //COLOURS
    controlColorTemplate slider_1_colors[5];
    controlColorTemplate combo_1_colors[4];
    controlColorTemplate textbox_1_colors[4];
    controlColorTemplate label_1_colors[1];
    controlColorTemplate imagebutton_1_colors[2];
    
    initDirectories();

    // LOAD SKIN XML DEF
    std::string xmlFilePath=currentSkinPath+"skin.xml"; 
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
    // APPLY COLOR TO ALLLLLLLLLLLLLLLLLLLLLLL
    //IMAGES
    stepseq_off= juce::ImageFileFormat::loadFrom(juce::File(imagePath+"stepseq_off.png"));
    stepseq_on=juce::ImageFileFormat::loadFrom(juce::File(imagePath+"stepseq_on.png"));
    plugin_banner=juce::ImageFileFormat::loadFrom(juce::File(imagePath+"banner_4.png"));

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


void ChopperAudioProcessorEditor::LoadXMLConfig(bool reloadSkin=true,bool reloadPatternsAndSequences=false,bool reloadParameters=true)
{    
    std::string xmlFilePath=configPath+"config.xml";
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
                                               
                        if ( (e->getTagName() == "skin") and reloadSkin) { currentSkin=paramName.toStdString(); /*e->setText("red");*/}
                    } 
                if (reloadPatternsAndSequences) {
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
                   if (reloadParameters) {
                    for (auto* e : rootElement->getChildByName("parameters")->getChildIterator())
                        {
                            //debug.setText(debug.getText()+"\n" +e->getTagName()  + " : " +  e->getAllSubText());
                            std::string paramName = e->getTagName().toStdString();
                            std::string paramValue = e->getAllSubText().toStdString();
                            if (paramName ==  "currentPattern" ) {seq_pattern_selected.setSelectedItemIndex(stoi(paramValue));audioProcessor.pseq_current->operator=(stoi(paramValue));/*step_seq_change();*/}
                            if (paramName ==  "currentSequence" ) {seq_sequence_selected.setSelectedItemIndex(stoi(paramValue));audioProcessor.sseq_current->operator=(stoi(paramValue));/*seq_pattern_change();*/}
                            if (paramName ==  "currenPatternData" ) {audioProcessor.pseq_data->operator=(stoi(paramValue));}
                            if (paramName ==  "sequencePosition" ) {}
                            if (paramName ==  "sequenceLength" ) {seq_length.setValue(stoi(paramValue));audioProcessor.sseq_length->operator=(stoi(paramValue));}
                            if (paramName ==  "patternDisplayRefresh" ) {audioProcessor.pseq_auto->operator=(stoi(paramValue));}
    
                            if (paramName ==  "clockDivision" ) {seq_clock.setSelectedItemIndex(stoi(paramValue));audioProcessor.clock_div->operator=(stoi(paramValue));}
                            if (paramName ==  "mode" ) {seq_mode.setSelectedItemIndex(stoi(paramValue));audioProcessor.seq_mode->operator=(stoi(paramValue));}
                            if (paramName ==  "enveloppeType" ) {seq_env.setSelectedItemIndex(stoi(paramValue));audioProcessor.seq_env->operator=(stoi(paramValue));}

                            if (paramName ==  "gateLength" ) {seq_gate_length.setValue(stof(paramValue));audioProcessor.pseq_gate_length->operator=(stof(paramValue));}
                            if (paramName ==  "dryWet" ) {mix__slider->setValue(stof(paramValue));audioProcessor.out_mix->operator=(stof(paramValue));mix__slider->setValue(audioProcessor.out_mix->get());}
                            if (paramName ==  "outputGain" ) {audioProcessor.out_gain->operator=(stof(paramValue));gain__slider->setValue(audioProcessor.out_gain->get());}
                        }
                    }
               // rootElement.writeTo (xmlFile(xmlFilePath)):
			} 
        }
    }
    if (reloadPatternsAndSequences) {
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

void ChopperAudioProcessorEditor::writeXMLConfig(bool updateSkin=true,bool updatePatternsAndSequences=true,bool updateParameters=true)
{
    LoadXMLConfig(not updateSkin,not updatePatternsAndSequences,not updateParameters);
   
    juce::XmlElement configFile ("choppah");// create root
    //////////////////////// OPTIONS ///////////////////////////////////////////
    juce::XmlElement* options = new juce::XmlElement ("options"); // create options
    juce::XmlElement* skin = new juce::XmlElement ("skin");      // create skin
    skin->addTextElement (currentSkin);                         // add value to skin
    options->addChildElement(skin);                             // add skin to options
    juce::XmlElement* version = new juce::XmlElement ("version");// create version
    version->addTextElement ("0.11");       // add value to version
    options->addChildElement(version);  // add version to options
    configFile.addChildElement (options); // add options to root
    /////////////////////// PLUGIN PARAMETERS ///////////////////////////////////////////
    juce::XmlElement* parameters = new juce::XmlElement ("parameters"); // create parameters collection    
    juce::XmlElement* par_pseq_current = new juce::XmlElement ("currentPattern"); // Init xmlElements
    juce::XmlElement* par_sseq_current= new juce::XmlElement ("currentSequence");
    juce::XmlElement* par_pseq_data= new juce::XmlElement ("currenPatternData");
    juce::XmlElement* par_seq_pos= new juce::XmlElement ("sequencePosition");
    juce::XmlElement* par_sseq_length= new juce::XmlElement ("sequenceLength");
    juce::XmlElement* par_pseq_auto= new juce::XmlElement ("patternDisplayRefresh");
    juce::XmlElement* par_clock_div= new juce::XmlElement ("clockDivision");
    juce::XmlElement* par_seq_mode= new juce::XmlElement ("mode");
    juce::XmlElement* par_seq_env= new juce::XmlElement ("enveloppeType");  
    juce::XmlElement* par_pseq_gate_length = new juce::XmlElement ("gateLength");
    juce::XmlElement* par_out_mix = new juce::XmlElement ("dryWet");
    juce::XmlElement* par_out_gain= new juce::XmlElement ("outputGain");
    par_pseq_current->addTextElement (std::__cxx11::to_string(audioProcessor.pseq_current->get())); // INTEGERS
    par_sseq_current->addTextElement (std::__cxx11::to_string(audioProcessor.sseq_current->get())); //copy values to XmlElements
    par_pseq_data->addTextElement (std::__cxx11::to_string(audioProcessor.pseq_data->get()));
    par_seq_pos->addTextElement (std::__cxx11::to_string(audioProcessor.seq_pos->get()));
    par_sseq_length->addTextElement (std::__cxx11::to_string(audioProcessor.sseq_length->get()));
    par_pseq_auto->addTextElement (std::__cxx11::to_string(audioProcessor.pseq_auto->get()));
    par_clock_div->addTextElement (std::__cxx11::to_string(audioProcessor.clock_div->getIndex())); // CHOICES (INTEGERS)
    par_seq_mode->addTextElement (std::__cxx11::to_string(audioProcessor.seq_mode->getIndex()));
    par_seq_env->addTextElement (std::__cxx11::to_string(audioProcessor.seq_env->getIndex()));  
    par_pseq_gate_length->addTextElement (std::__cxx11::to_string(audioProcessor.pseq_gate_length->get()));   // FLOATS
    par_out_mix->addTextElement (std::__cxx11::to_string(audioProcessor.out_mix->get()));
    par_out_gain->addTextElement (std::__cxx11::to_string(audioProcessor.out_gain->get()));
    parameters->addChildElement(par_pseq_current);
    parameters->addChildElement(par_sseq_current); //copy values to parameter collection
    parameters->addChildElement(par_pseq_data);
    parameters->addChildElement(par_seq_pos);
    parameters->addChildElement(par_sseq_length);
    parameters->addChildElement(par_pseq_auto);
    parameters->addChildElement(par_clock_div); // CHOICES (INTEGERS)
    parameters->addChildElement(par_seq_mode);
    parameters->addChildElement(par_seq_env);
    parameters->addChildElement(par_pseq_gate_length);   // FLOATS
    parameters->addChildElement(par_out_mix);
    parameters->addChildElement(par_out_gain);
    configFile.addChildElement (parameters);// add parameters collection to root
    //////////////////////// PATTERNS ///////////////////////////////////////////
    juce::XmlElement* patternes = new juce::XmlElement ("patterns");  // create pattern collection
    for (int i=0;i<16;i++) {
        juce::XmlElement* pattern = new juce::XmlElement ("pattern");  // create pattern
        pattern->setTagName("pattern" + std::__cxx11::to_string(i+1)); // change pattern tag to numbered one
        bool ptrn[16];for (int j=0;j<16;j++) {ptrn[j]=patterns[i][15-j];}  //get & invert pattern[i]             
        pattern->addTextElement (std::__cxx11::to_string(bitArrayToInt32(ptrn,16))); // compute & add pattern int
        patternes->addChildElement(pattern); //add pattern to pattern collection
    } 
    configFile.addChildElement (patternes);// add pattern collection to root
    //////////////////////// SEQUENCES ///////////////////////////////////////////
    juce::XmlElement* sequencess = new juce::XmlElement ("sequences"); // create sequence collection
    for (int i=0;i<16;i++) {
        juce::XmlElement* seq = new juce::XmlElement ("sequence");  // create sequence
        seq->setTagName("sequence" + std::__cxx11::to_string(i+1)); // change sequence tag name to numbered one
        long int sequenceCode=0;//  init sequence code
        for (int k=0;k<16;k++) { sequenceCode+=((long)(sequences[i][15-k]) << 4*k );} // calculate sequence code : each sequence step is stored on 4 bits so we create a (16 steps x 4 bits = ) 64 bit int 
        seq->addTextElement (std::__cxx11::to_string(sequenceCode)); // write in sequence
        sequencess->addChildElement(seq); // add sequence to sequence collection
    } 
    configFile.addChildElement (sequencess);// add sequence collection to root
           
    auto xmlString = configFile.toString();// now we can turn the whole thing into textual XML
    std::ofstream file(configPath+"config.xml"); // create output file
    file<<xmlString; // append xml data to file

    //debug.setText(xmlString);
}

