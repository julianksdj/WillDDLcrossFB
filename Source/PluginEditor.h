/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class WillDDLAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                     public juce::Slider::Listener,
                                     public juce::Button::Listener
{
public:
    WillDDLAudioProcessorEditor (WillDDLAudioProcessor&);
    ~WillDDLAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void sliderValueChanged (juce::Slider *slider) override
    {
        if(slider == &delayTimeSlider)
        {
            audioProcessor.setDelayTime(delayTimeSlider.getValue()) ;
        }
        else if(slider == &feedbackSlider)
        {
            audioProcessor.setFeedback(feedbackSlider.getValue()) ;
        }
        else if(slider == &wetLevelSlider)
        {
            audioProcessor.setWetLevel(wetLevelSlider.getValue()) ;
        }
        audioProcessor.cookVariables();
    };
    
    void buttonClicked (juce::Button* button) override
    {
        if (button == &crossButton)
        {
            if (!audioProcessor.getCross())
            {
                crossButton.setColour (juce::TextButton::buttonColourId, juce::Colours::green);
            }
            else
            {
                crossButton.setColour (juce::TextButton::buttonColourId, juce::Colours::red);
            }
            audioProcessor.switchCross();
        }
    };
    
    juce::Slider delayTimeSlider;
    juce::Slider feedbackSlider;
    juce::Slider wetLevelSlider;
    
    juce::Label delayTimeLabel;
    juce::Label feedbackLabel;
    juce::Label wetLevelLabel;
    
    juce::TextButton crossButton;
    

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    WillDDLAudioProcessor& audioProcessor;
    bool buttonOn;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WillDDLAudioProcessorEditor)
};
