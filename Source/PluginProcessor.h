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
class WillDDLAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    WillDDLAudioProcessor();
    ~WillDDLAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
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
    
    float getDelayTime(){
        return delayTime;
    };
    float getFeedback(){
        return feedbackParam;
    };
    float getWetLevel(){
        return wetLevelParam;
    };
    
    void setDelayTime(float dt){
        delayTime = dt;
    };
    void setFeedback(float fb){
        feedbackParam = fb;
    };
    void setWetLevel(float wl){
        wetLevelParam = wl;
    };
    
    void cookVariables(){
        delaySamples = delayTime*(currentSampleRate/1000.0);
        feedback = feedbackParam/100.0;
        wetLevel = wetLevelParam/100.0;
        // subtract to make read index
        readIndex[0] = writeIndex[0] - (int)delaySamples; // cast as int!
        readIndex[1] = writeIndex[1] - (int)delaySamples;
        // check and wrap BACKWARDS if the index is negative
        if (readIndex[0] < 0)
            readIndex[0] += bufferSize;     // amount of wrap is Read + Length
        if (readIndex[1] < 0)
            readIndex[1] += bufferSize;
    };
    
    void resetDelay()
    {
        // flush buffer
        if(pBufferL)
            memset(pBufferL, 0, bufferSize*sizeof(float));
        if(pBufferR)
            memset(pBufferR, 0, bufferSize*sizeof(float));
        // init read/write indices
        writeIndex[0] = 0; // reset the Write index to top
        writeIndex[1] = 0;
        readIndex[0] = 0; // reset the Read index to top
        readIndex[1] = 0;
    };
    
    void switchCross()
    {
        cross = !cross;
    };
    bool getCross()
    {
        return cross;
    };

private:
    
    float currentSampleRate;
    float delayTime;
    float delaySamples;
    float feedback, feedbackParam;
    float wetLevel, wetLevelParam;
    
    // circular buffer
    float* pBufferL;
    float* pBufferR;
    int readIndex[2];
    int writeIndex[2];
    int bufferSize;
    
    //cross button enable/disable
    bool cross;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WillDDLAudioProcessor)
};
