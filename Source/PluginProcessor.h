/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
/**
*/
class ChopperAudioProcessor  : public juce::AudioProcessor
                            
{
public:
    //==============================================================================
    ChopperAudioProcessor();
    ~ChopperAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override ;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void generateGateMap(int patternData);
    long int getDAWbarcount() const;
    double getDAWppqlastbar() const;
    double getDAWppq() const;
    void updatePattern(int patternIndex,int patternData);
    void updateEnv(int env);
    void initGatesMap();
    void update_pat();

    //GUI controls
    float gui_out_mix;
    //parameters
    juce::AudioParameterInt* pseq_current;
    juce::AudioParameterInt* sseq_current;
    juce::AudioParameterFloat* pseq_gate_length;
    juce::AudioParameterChoice* seq_mode;
    juce::AudioParameterChoice* clock_div;
    juce::AudioParameterChoice* seq_env;
    juce::AudioParameterInt* pseq_data;
    juce::AudioParameterInt* seq_pos;
    juce::AudioParameterInt* sseq_length;
    juce::AudioParameterInt* pseq_auto;

    juce::AudioParameterFloat* out_mix;
    juce::AudioParameterFloat* out_gain;

    juce::SmoothedValue<float,juce::ValueSmoothingTypes::Multiplicative> smoothChop;
    juce::SmoothedValue<float,juce::ValueSmoothingTypes::Multiplicative> smoothMix;
    //juce::SmoothedValue<float> smoothOut;
    
    juce::AudioBuffer<float> drybuffer;

    float gateMap[64];
    float gateMaps[16][64];
    int sequences[16];
    int pattern_seqs[16][16];
private:
    juce::dsp::DryWetMixer<float> dryWetMixer;
    long int barcount;
    double ppq;
    double ppqlastbar;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChopperAudioProcessor)
};
