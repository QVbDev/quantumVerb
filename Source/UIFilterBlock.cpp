/*
  ==============================================================================

    UIFilterBlock.cpp

  ==============================================================================
*/

#include "UIFilterBlock.h"
#include "PluginEditor.h"

namespace reverb
{

    //==============================================================================
    /**
    * @brief Constructs a UIFilterBlock object
    *
    * Creates a UIFilterBlock and each of its components. Constructs a
    * building block for the UI. This block includes all sliders required
    * for a filter as well as does most of its configuration. It also adds its
    * parameters to the AudioProcessorValueTreeState.
    */
    UIFilterBlock::UIFilterBlock(AudioProcessor& processor, int index, const juce::String& displayedName)
        : UIBlock(3, 2, "Filter", displayedName)
    {
        // Sliders
        freq.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        q.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        gain.setSliderStyle(juce::Slider::RotaryVerticalDrag);

        freq.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
        q.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
        gain.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);

        freq.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colour(0x00000000));
        q.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colour(0x00000000));
        gain.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colour(0x00000000));

        juce::String filterIDPrefix = processor.PID_FILTER_PREFIX + std::to_string(index);

        freq.setComponentID(filterIDPrefix + processor.PID_FILTER_FREQ_SUFFIX);
        q.setComponentID(filterIDPrefix + processor.PID_FILTER_Q_SUFFIX);
        gain.setComponentID(filterIDPrefix + processor.PID_FILTER_GAIN_SUFFIX);

        // Labels
        freqLabel.setText("frequency", juce::NotificationType::dontSendNotification);
        freqLabel.setJustificationType(juce::Justification::centredBottom);

        qLabel.setText("q factor", juce::NotificationType::dontSendNotification);
        qLabel.setJustificationType(juce::Justification::centredBottom);

        gainLabel.setText("amplitude", juce::NotificationType::dontSendNotification);
        gainLabel.setJustificationType(juce::Justification::centredBottom);

        freqLabel.attachToComponent(&freq, false);
        qLabel.attachToComponent(&q, false);
        gainLabel.attachToComponent(&gain, false);

        // Attachments
        freqAttachment.reset(new SliderAttachment(processor.parameters,
                                                  freq.getComponentID(),
                                                  freq));

        qAttachment.reset(new SliderAttachment(processor.parameters,
                                               q.getComponentID(),
                                               q));

        gainAttachment.reset(new SliderAttachment(processor.parameters,
                                                  gain.getComponentID(),
                                                  gain));

        // Tweak value box appearance
        freq.setNumDecimalPlacesToDisplay(0);
        freq.setTextValueSuffix(" Hz");

        q.setNumDecimalPlacesToDisplay(2);

        gain.setNumDecimalPlacesToDisplay(2);
        gain.setTextValueSuffix(" dB");

        // Skew factor
        freq.setSkewFactor(1.0, false);
        q.setSkewFactor(1.0, false);
        gain.setSkewFactor(0.5, false); // Skew factor of 0.5 on all gain filter sliders

        // Add sliders
        addAndMakeVisible(freq);
        addAndMakeVisible(q);
        addAndMakeVisible(gain);
    }

    //==============================================================================
    void UIFilterBlock::paint(juce::Graphics& g)
    {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

        g.setColour(juce::Colours::white);
        g.setFont(15.0f);

        juce::Rectangle<int> bounds(getLocalBounds());
        getLookAndFeel().drawGroupComponentOutline(g, bounds.getWidth(), bounds.getHeight(), getText(),
            juce::Justification(juce::Justification::centredTop), *this);
    }

    //==============================================================================
    /**
    * @brief Manages the layout of UIFilterBlock when the block is resized
    *
    * This function defines all the relative positioning of the various UIFilterBlock
    * elements.
    */
    void UIFilterBlock::resized()
    {
        juce::Rectangle<int> bounds(getLocalBounds());

        int height = bounds.getHeight();
        int padding = (int)(height * 0.06);
        bounds.reduce(padding, padding);

        // Distribute elements in columns
        auto cells = getComponentCells(bounds);

        freq.setBounds(cells[0]);
        q.setBounds(cells[1]);
        gain.setBounds(cells[2]);
    }

}
