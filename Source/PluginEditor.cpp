/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WillDDLAudioProcessorEditor::WillDDLAudioProcessorEditor (WillDDLAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (300, 200);
    
    addAndMakeVisible (delayTimeSlider);
    delayTimeSlider.setNormalisableRange(juce::NormalisableRange<double>(0.f, 2000.f, 1.f, 1.f));
    delayTimeSlider.addListener (this);
    delayTimeSlider.setValue(audioProcessor.getDelayTime());
    delayTimeLabel.setText ("Delay Time (ms)", juce::dontSendNotification);
    delayTimeLabel.setJustificationType(juce::Justification::centred);
    delayTimeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (delayTimeLabel);
    delayTimeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    delayTimeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 80, 15);
    
    addAndMakeVisible (feedbackSlider);
    feedbackSlider.setNormalisableRange(juce::NormalisableRange<double>(0.f, 100.f, 1.f,  1.f));
    feedbackSlider.addListener (this);
    feedbackSlider.setValue(audioProcessor.getFeedback());
    feedbackLabel.setText ("Feedback", juce::dontSendNotification);
    feedbackLabel.setJustificationType(juce::Justification::centred);
    feedbackLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (feedbackLabel);
    feedbackSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    feedbackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 80, 15);
    
    addAndMakeVisible (wetLevelSlider);
    wetLevelSlider.setNormalisableRange(juce::NormalisableRange<double>(0.f, 100.f, 1.f,  1.f));
    wetLevelSlider.addListener (this);
    wetLevelSlider.setValue(audioProcessor.getWetLevel());
    wetLevelLabel.setText ("Dry/Wet (%)", juce::dontSendNotification);
    wetLevelLabel.setJustificationType(juce::Justification::centred);
    wetLevelLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (wetLevelLabel);
    wetLevelSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    wetLevelSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 80, 15);
    
    addAndMakeVisible (crossButton);
    crossButton.setButtonText ("Cross");
    crossButton.addListener(this);
    if (audioProcessor.getCross())
        crossButton.setColour (juce::TextButton::buttonColourId, juce::Colours::green);
    else
        crossButton.setColour (juce::TextButton::buttonColourId, juce::Colours::red);
}

WillDDLAudioProcessorEditor::~WillDDLAudioProcessorEditor()
{
}

//==============================================================================
void WillDDLAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void WillDDLAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    const int width = getWidth();
    const int height = getHeight();
    const int border = 10;
    
    const int sliderWidth = width/4;
    const int sliderHeight = height-4*border;
    
    delayTimeSlider.setBounds(border, border, sliderWidth-border, sliderHeight);
    feedbackSlider.setBounds(border+sliderWidth, border, sliderWidth-border, sliderHeight);
    wetLevelSlider.setBounds(border+sliderWidth*2, border, sliderWidth-border, sliderHeight);
    
    delayTimeLabel.setBounds(border, sliderHeight+15, sliderWidth-border, 20);
    feedbackLabel.setBounds(border+sliderWidth, sliderHeight+15, sliderWidth-border, 20);
    wetLevelLabel.setBounds(border+sliderWidth*2, sliderHeight+15, sliderWidth-border, 20);
    
    crossButton.setBounds(border+sliderWidth*3, border+sliderHeight/3, 50,50);
}
