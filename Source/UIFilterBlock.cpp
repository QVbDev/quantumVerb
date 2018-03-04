/*
  ==============================================================================

    UIFilterBlock.cpp

  ==============================================================================
*/

#include "UIFilterBlock.h"
#include "PluginEditor.h"

namespace reverb
{
    /**
    * @brief Constructs a UIFilterBlock object
    *
    * Creates a UIFilterBlock and each of its components. Constructs a
    * building block for the UI. This block includes all sliders required
    * for a filter as well as does most of its configuration. It also adds its 
    * parameters to the AudioProcessorValueTreeState.
    */
    //==============================================================================
    UIFilterBlock::UIFilterBlock(AudioProcessor& processor, int index)
        : UIBlock(3, 2)
    {
        // Sliders
        freq.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        q.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        gain.setSliderStyle(juce::Slider::RotaryVerticalDrag);

        freq.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        q.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        gain.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

        juce::String filterIDPrefix = processor.PID_FILTER_PREFIX + std::to_string(index);

        freq.setComponentID(filterIDPrefix + processor.PID_FILTER_FREQ_SUFFIX);
        q.setComponentID(filterIDPrefix + processor.PID_FILTER_Q_SUFFIX);
        gain.setComponentID(filterIDPrefix + processor.PID_FILTER_GAIN_SUFFIX);

        // Labels
        qLabel.setText("q factor", juce::NotificationType::dontSendNotification);
        qLabel.setJustificationType(juce::Justification::centredBottom);

        freqLabel.setText("frequency", juce::NotificationType::dontSendNotification);
        freqLabel.setJustificationType(juce::Justification::centredBottom);

        gainLabel.setText("amplitude", juce::NotificationType::dontSendNotification);
        gainLabel.setJustificationType(juce::Justification::centredBottom);

        qLabel.attachToComponent(&q, false);
        freqLabel.attachToComponent(&freq, false);
        gainLabel.attachToComponent(&gain, false);

        // Attachments
        gainAttachment.reset(new SliderAttachment(processor.parameters,
                                                  freq.getComponentID(),
                                                  freq));

        qAttachment.reset(new SliderAttachment(processor.parameters,
                                               q.getComponentID(),
                                               q));

        gainAttachment.reset(new SliderAttachment(processor.parameters,
                                                  gain.getComponentID(),
                                                  gain));

        // Add sliders
        addAndMakeVisible(q);
        addAndMakeVisible(freq);
        addAndMakeVisible(gain);

        // Default Values
        freq.setValue(808.0f);
        q.setValue(1.06f);
        gain.setValue(juce::Decibels::decibelsToGain(-6));
    }

    //==============================================================================
    void UIFilterBlock::paint(juce::Graphics& g)
    {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

        g.setColour(juce::Colours::white);
        g.setFont(15.0f);
    }
    /**
    * @brief Manages the layout of UIFilterBlock when the block is resized
    *
    * This function defines all the relative positioning of the various UIFilterBlock
    * elements.
    */
    //==============================================================================
    void UIFilterBlock::resized()
    {
        juce::Rectangle<int> bounds(getLocalBounds());

        // Draw frame
        // TODO: Make this look nice

        // Distribute elements in columns
        auto cells = getComponentCells(bounds);

        freq.setBounds(cells[0]);
        q.setBounds(cells[1]);
        gain.setBounds(cells[2]);
    }

}
