/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

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
    void generateGateMap(int patternData) {
        int size=16;
        int step=4;
        int pattern=pseq_current->get()-1;
        float gateMap_arr[size*step];
        const float envs[3][step]={
            {1.0,1.0,1.0,1.0}, // FLAT 
            {1.0,1.0,0.66,0.33}, // SHARP
            {1.0,0.66,0.33,0.0} // TRI                  
        };
        int env=seq_env->getIndex(); 
        bool gates[size];

        for (int i=0;i<size;i++)
        {
            gates[i]=((patternData >> i) & 1);
            for (int j=0;j<step;j++) {
                if(gates[i] == true) {
                  gateMap[i*4+j]=envs[env][j];
                  gateMaps[pattern][i*4+j]=envs[env][j];
                } else {
                  gateMap[i*4+j]=0.0;
                  gateMaps[pattern][i*4+j]=0.0;
                }
            }
        }

       // for (int i=0;i<size*step;i++)
    }
    long int getDAWbarcount() const
    {
        return barcount;
    }
    double getDAWppqlastbar() const
    {
        return ppqlastbar;
    }
    double getDAWppq() const
    {
        return ppq;
    }
    void updatePattern(int patternIndex,int patternData)
    {
        sequences[patternIndex]=patternData;
        pseq_current->operator=(patternIndex+1);
        pseq_data->operator=(/*pattern_seqs[patternIndex]*/patternData);
        generateGateMap(patternData);
    }
    void updateEnv(int env)
    {
        seq_env->operator=(env);
        generateGateMap(pseq_data->get());
    }
    void initGatesMap()
    {
        for (int i=0;i<sizeof(sequences)/sizeof(sequences[0]);i++)
        {
            generateGateMap(sequences[i]);
            for (int j=0;j<sizeof(gateMap)/sizeof(gateMap[0]);j++) {
                gateMaps[i][j]=gateMap[j];
            }
        }
        //pseq_current->operator=(1);
    }
    void update_pat()
    {
       int patternData=sequences[pseq_current->get()-1];
       pseq_data->operator=(patternData); 
       generateGateMap(patternData);
    }
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

    juce::SmoothedValue<float> smooth;

    float gateMap[64];
    float gateMaps[16][64];
    int sequences[16];
    int pattern_seqs[16][16];
    
    
private:
    
    long int barcount;
    double ppq;
    double ppqlastbar;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChopperAudioProcessor)
};
