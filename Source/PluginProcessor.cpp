/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WillDDLAudioProcessor::WillDDLAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    // reset circular buffer
    readIndex[0] = 0;
    readIndex[1] = 0;
    writeIndex[0] = 0;
    writeIndex[1] = 0;
    // no buffer yet because we don’t have a sample rate yet
    pBufferL = NULL;
    pBufferR = NULL;
    bufferSize = 0;
}

WillDDLAudioProcessor::~WillDDLAudioProcessor()
{
    // delete buffer if it exists
    if(pBufferL)
        delete [] pBufferL;
    if(pBufferR)
        delete [] pBufferR;
}

//==============================================================================
const juce::String WillDDLAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WillDDLAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool WillDDLAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool WillDDLAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double WillDDLAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WillDDLAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int WillDDLAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WillDDLAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String WillDDLAudioProcessor::getProgramName (int index)
{
    return {};
}

void WillDDLAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void WillDDLAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    currentSampleRate = sampleRate;
    
    bufferSize = 2 * currentSampleRate;     // 2 seconds delay @ fs

    pBufferL = new float[bufferSize];
    pBufferR = new float[bufferSize];
    
    resetDelay();
    
    setFeedback(50.f);
    setDelayTime(1000.f);
    setWetLevel(50.f);
    cookVariables();
    
    cross = false;
}

void WillDDLAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WillDDLAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void WillDDLAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        // ..do something to the data...
        for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            // Do LEFT (MONO) Channel
            // Read the Input
            float xn = channelData[sample];
            // Read the output of the delay at readIndex
            float yn;
            if (cross)
            {
                if(channel == 0)
                    yn = pBufferR[readIndex[channel]];
                else
                    yn = pBufferL[readIndex[channel]];
            }
            else
            {
                if(channel == 0)
                    yn = pBufferL[readIndex[channel]];
                else
                    yn = pBufferR[readIndex[channel]];
            }
            // if zero delay, just pass the input to output
            if(delaySamples == 0)
                yn = xn;
            // write the input to the delay
            if(channel == 0)
                pBufferL[writeIndex[channel]] = xn + feedback*yn;
            else
                pBufferR[writeIndex[channel]] = xn + feedback*yn;
            // create the wet/dry mix and write to the output buffer
            // dry = 1 - wet
            channelData[sample] = wetLevel * yn + (1.f - wetLevel)*xn;
            
            // incremnent the pointers and wrap if necessary
            writeIndex[channel]++;
            if(writeIndex[channel] >= bufferSize)
                 writeIndex[channel] = 0;
            readIndex[channel]++;
            if(readIndex[channel] >= bufferSize)
                 readIndex[channel] = 0;
        }
    }
}

//==============================================================================
bool WillDDLAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* WillDDLAudioProcessor::createEditor()
{
    return new WillDDLAudioProcessorEditor (*this);
}

//==============================================================================
void WillDDLAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void WillDDLAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WillDDLAudioProcessor();
}
