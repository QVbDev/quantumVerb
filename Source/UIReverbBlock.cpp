/*
  ==============================================================================

    UIReverbBlock.cpp

  ==============================================================================
*/

#include "UIReverbBlock.h"
#include "PluginEditor.h"

namespace reverb
{

    //==============================================================================
    UIReverbBlock::UIReverbBlock(AudioProcessor& processor)
        : UIBlock(5, 2)
    {
        // Sliders
        irLength.setSliderStyle(juce::Slider::Rotary);
        preDelay.setSliderStyle(juce::Slider::Rotary);
        irGain.setSliderStyle(juce::Slider::Rotary);
        outGain.setSliderStyle(juce::Slider::Rotary);
        wetRatio.setSliderStyle(juce::Slider::Rotary);

        irLength.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        preDelay.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        irGain.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        outGain.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        wetRatio.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

        irLength.setComponentID(processor.PID_IR_LENGTH);
        preDelay.setComponentID(processor.PID_PREDELAY);
        irGain.setComponentID(processor.PID_IR_GAIN);
        outGain.setComponentID(processor.PID_AUDIO_OUT_GAIN);
        wetRatio.setComponentID(processor.PID_WETRATIO);

        // Labels
        irLengthLabel.setText("reverb length", juce::NotificationType::dontSendNotification);
        irLengthLabel.setJustificationType(juce::Justification::centredBottom);

        preDelayLabel.setText("predelay", juce::NotificationType::dontSendNotification);
        preDelayLabel.setJustificationType(juce::Justification::centredBottom);

        irGainLabel.setText("reverb volume", juce::NotificationType::dontSendNotification);
        irGainLabel.setJustificationType(juce::Justification::centredBottom);

        outGainLabel.setText("output volume", juce::NotificationType::dontSendNotification);
        outGainLabel.setJustificationType(juce::Justification::centredBottom);

        wetRatioLabel.setText("wet ratio", juce::NotificationType::dontSendNotification);
        wetRatioLabel.setJustificationType(juce::Justification::centredBottom);

        irLengthLabel.attachToComponent(&irLength, false);
        preDelayLabel.attachToComponent(&preDelay, false);
        irGainLabel.attachToComponent(&irGain, false);
        outGainLabel.attachToComponent(&outGain, false);
        wetRatioLabel.attachToComponent(&wetRatio, false);

        // Attachments
        irLengthAttachment.reset(new SliderAttachment(processor.parameters,
                                                      irLength.getComponentID(),
                                                      irLength));
        
        preDelayAttachment.reset(new SliderAttachment(processor.parameters,
                                                      preDelay.getComponentID(),
                                                      preDelay));

        irGainAttachment.reset(new SliderAttachment(processor.parameters,
                                                    irGain.getComponentID(),
                                                    irGain));

        outGainAttachment.reset(new SliderAttachment(processor.parameters,
                                                     outGain.getComponentID(),
                                                     outGain));

        wetRatioAttachment.reset(new SliderAttachment(processor.parameters,
                                                      wetRatio.getComponentID(),
                                                      wetRatio));

        // Add sliders
        addAndMakeVisible(irLength);
        addAndMakeVisible(preDelay);
        addAndMakeVisible(irGain);
        addAndMakeVisible(outGain);
        addAndMakeVisible(wetRatio);
    }

    //==============================================================================
    void reverb::UIReverbBlock::paint(juce::Graphics& g)
    {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

        g.setColour(juce::Colours::white);
        g.setFont(15.0f);
    }

    //==============================================================================
    void reverb::UIReverbBlock::resized()
    {
        juce::Rectangle<int> bounds(getLocalBounds());

        // Draw frame
        // TODO: Make this look nice

        // Distribute child elements in columns
        auto cells = getComponentCells(bounds);

        irLength.setBounds(cells[0]);
        preDelay.setBounds(cells[1]);
        irGain.setBounds(cells[2]);
        outGain.setBounds(cells[3]);
        wetRatio.setBounds(cells[4]);
    }

}
