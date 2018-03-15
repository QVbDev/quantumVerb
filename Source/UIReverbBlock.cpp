/*
  ==============================================================================

    UIReverbBlock.cpp

  ==============================================================================
*/

#include "UIReverbBlock.h"
#include "PluginEditor.h"

namespace reverb
{
    /**
    * @brief Constructs an UIReverbBlock object
    *
    * Creates a UIReverbBlock and each of its components. Constructs a
    * building block for the UI. This block includes all sliders required
    * for the impulse response parameters and output parammeters. It also
    * does most of the configuration for these sliders. It also adds its 
    * parameters to the AudioProcessorValueTreeState.
    */
    //==============================================================================
    UIReverbBlock::UIReverbBlock(AudioProcessor& processor)
        : UIBlock(5, 2)
    {
        // Sliders
        irLength.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        preDelay.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        irGain.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        outGain.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        wetRatio.setSliderStyle(juce::Slider::RotaryVerticalDrag);

        irLength.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
        preDelay.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
        irGain.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
        outGain.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
        wetRatio.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);

        irLength.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colour(0x00000000));
        preDelay.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colour(0x00000000));
        irGain.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colour(0x00000000));
        outGain.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colour(0x00000000));
        wetRatio.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colour(0x00000000));

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

        // Default values
        irLength.setValue(2.5f);
        preDelay.setValue(500.0f);
        irGain.setValue(0.5f);
        outGain.setValue(0.0f);
        wetRatio.setValue(0.5f);
    }

    //==============================================================================
    void reverb::UIReverbBlock::paint(juce::Graphics& g)
    {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

        g.setColour(juce::Colours::white);
        g.setFont(15.0f);
    }

    /**
    * @brief Manages the layout of UIReverbBlock when the block is resized
    *
    * This function defines all the relative positioning of the various UIReverbBlock
    * elements.
    */
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
