/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ChopperAudioProcessor::ChopperAudioProcessor()
     : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)                     
                                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       )   
{
    addParameter(pseq_current= new juce::AudioParameterInt ("pseq_current","Pattern#",1, 16, 1) );
    addParameter(sseq_current= new juce::AudioParameterInt ("sseq_current","Sequence#",1, 16, 1) );
    addParameter (pseq_gate_length = new juce::AudioParameterFloat ("pseq_gate_length","Gate length",0.0f, 1.0f, 0.5f));
    addParameter(pseq_data= new juce::AudioParameterInt ("pseq_seq","Pattern data",0x0, 0xFFFF, 0xAAAA) );
    addParameter(seq_pos= new juce::AudioParameterInt ("seq_pos","Pattern position",1, 16, 1) );
    addParameter(sseq_length= new juce::AudioParameterInt ("sseq_length","Sequence length",2, 16, 4) );
    addParameter(pseq_auto= new juce::AudioParameterInt ("pseq_auto","Pattern autorefresh",0, 1, 1) );

    addParameter (clock_div = new juce::AudioParameterChoice ("clock_div", "Clock division", { "1","1/2","1/4","1/8","1/16","1/32" }, 0));
    addParameter (seq_mode = new juce::AudioParameterChoice ("seq_mode", "Sequencer mode", { "Pattern","Sequence" }, 0));
    addParameter (seq_env = new juce::AudioParameterChoice ("seq_env", "Step enveloppe", { "flat","sharp","tri" }, 0));

    addParameter (out_mix = new juce::AudioParameterFloat ("out_mix","Dry/Wet",0.0f, 1.0f, 0.5f));
    addParameter (out_gain = new juce::AudioParameterFloat ("out_gain","Gain Out",0.0f, 10.0f, 1.0f));

    sseq_length->operator=(4);
    out_mix->operator=(0.5);
    
    dryWetMixer.setWetMixProportion(out_mix->get());
    dryWetMixer.setWetLatency(0.0f);
    
}

ChopperAudioProcessor::~ChopperAudioProcessor()
{
}

//==============================================================================
const juce::String ChopperAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ChopperAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ChopperAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ChopperAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ChopperAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ChopperAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ChopperAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ChopperAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ChopperAudioProcessor::getProgramName (int index)
{
    return {};
}

void ChopperAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ChopperAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    drybuffer.setSize(2, samplesPerBlock);
}

void ChopperAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ChopperAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ChopperAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    float seqStepValue,lastSeqStepValue;
    float gain=out_gain->get();
    float mixLevel=out_mix->get();
    float lastDryValue=smoothMix.getNextValue();
    //float lastOutValue=smoothOut.getNextValue();
    smoothMix.setTargetValue((1-mixLevel)*gain);
    //smoothOut.setTargetValue(gain);
    drybuffer.copyFrom(0, 0, buffer, 0, 0, buffer.getNumSamples());
    drybuffer.copyFrom(1, 0, buffer, 1, 0, buffer.getNumSamples());
    dryWetMixer.setWetMixProportion(mixLevel);

    juce::AudioPlayHead* phead = getPlayHead();
    if (phead != nullptr)
    {
        auto playposinfo=phead->getPosition();
        ppq=*(*playposinfo).getPpqPosition();
        ppqlastbar=*(*playposinfo).getPpqPositionOfLastBarStart();
        int step=int(std::floor((ppq/4-ppqlastbar/4)*64));
        
        int selected_pattern=pseq_current->get()-1;

        if (seq_mode->getIndex() == 1) {
            const int seqLength=sseq_length->get(); 
            int sequenceNumber=sseq_current->get()-1;
            int seqIndex;
            if (int(ppq/4)>=seqLength) {seqIndex=int(std::floor(ppq/4)-seqLength*std::floor(ppq/(4*seqLength)));} else {seqIndex=int(std::floor(ppq/4));}
           
            selected_pattern=pattern_seqs[sequenceNumber][seqIndex];
            if (selected_pattern>=15) {selected_pattern=15;}
            if (pseq_auto->get()==1) {pseq_current->operator=(selected_pattern+1);}            
        }
        pseq_data->operator=(sequences[selected_pattern]);
        seqStepValue=gateMaps[selected_pattern][step]*gain;
        lastSeqStepValue=smoothChop.getNextValue();
        smoothChop.setTargetValue(seqStepValue);
    }

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {   
        auto* channelData = buffer.getWritePointer (channel);
        
        //drybuffer.copyFrom(channel, 0, buffer, channel, 0, buffer.getNumSamples());
        //auto drybuffer=buffer.copyFrom (1, 0, buffer.getReadPointer (0), buffer.getNumSamples());
        //buffer.applyGain(smooth.getNextValue());
        
        drybuffer.applyGainRamp(channel,0,drybuffer.getNumSamples(),lastDryValue,smoothMix.getNextValue());
        dryWetMixer.pushDrySamples(drybuffer); // Push the unprocessed dry samples
        buffer.applyGainRamp(channel,0,buffer.getNumSamples(),lastSeqStepValue,smoothChop.getNextValue());
        dryWetMixer.mixWetSamples(buffer); // Mix the processed wet samples
        
        //buffer.applyGainRamp(channel,0,buffer.getNumSamples(),lastOutValue,smoothOut.getNextValue());
    }
    buffer.addFrom(0, 0, drybuffer, 0, 0, buffer.getNumSamples());
    buffer.addFrom(1, 0, drybuffer, 1, 0, buffer.getNumSamples());
}

//==============================================================================
bool ChopperAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ChopperAudioProcessor::createEditor()
{
    return new ChopperAudioProcessorEditor (*this);
}

//==============================================================================
void ChopperAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream (destData, true).writeInt (*pseq_current);
    juce::MemoryOutputStream (destData, true).writeInt (*sseq_current);
    juce::MemoryOutputStream (destData, true).writeInt (*pseq_data);
    juce::MemoryOutputStream (destData, true).writeInt (*seq_pos);
    juce::MemoryOutputStream (destData, true).writeInt (*sseq_length);
    juce::MemoryOutputStream (destData, true).writeInt (*pseq_auto);

    juce::MemoryOutputStream (destData, true).writeInt (*clock_div);
    juce::MemoryOutputStream (destData, true).writeInt (*seq_mode);
    juce::MemoryOutputStream (destData, true).writeInt (*seq_env);
    
    juce::MemoryOutputStream (destData, true).writeFloat (*pseq_gate_length);   
    juce::MemoryOutputStream (destData, true).writeFloat (*out_mix);
    juce::MemoryOutputStream (destData, true).writeFloat (*out_gain);
}

void ChopperAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    *pseq_current=juce::MemoryInputStream (data, static_cast<size_t> (sizeInBytes), false).readInt();
    *sseq_current=juce::MemoryInputStream (data, static_cast<size_t> (sizeInBytes), false).readInt();
    *pseq_data=juce::MemoryInputStream (data, static_cast<size_t> (sizeInBytes), false).readInt();
    *sseq_length=juce::MemoryInputStream (data, static_cast<size_t> (sizeInBytes), false).readInt();
    *pseq_auto=juce::MemoryInputStream (data, static_cast<size_t> (sizeInBytes), false).readInt();
    //*seq_pos=juce::MemoryInputStream (data, static_cast<size_t> (sizeInBytes), false).readInt();

    *clock_div=juce::MemoryInputStream (data, static_cast<size_t> (sizeInBytes), false).readInt();
    *seq_mode=juce::MemoryInputStream (data, static_cast<size_t> (sizeInBytes), false).readInt();
    *seq_env=juce::MemoryInputStream (data, static_cast<size_t> (sizeInBytes), false).readInt();
     
    *pseq_gate_length = juce::MemoryInputStream (data, static_cast<size_t> (sizeInBytes), false).readFloat();
    *out_mix = juce::MemoryInputStream (data, static_cast<size_t> (sizeInBytes), false).readFloat();
    *out_gain = juce::MemoryInputStream (data, static_cast<size_t> (sizeInBytes), false).readFloat();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChopperAudioProcessor();
}
