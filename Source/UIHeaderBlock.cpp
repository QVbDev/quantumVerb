/*
  ==============================================================================

    UIHeaderBlock.cpp

  ==============================================================================
*/

#include "UIHeaderBlock.h"
#include "PluginEditor.h"

namespace reverb
{

    //==============================================================================
    UIHeaderBlock::UIHeaderBlock(AudioProcessor& p)
        : UIBlock(3, 3, "Header", "reverb settings"),
        parameters(p.parameters),
        previousSelectedIRs(juce::StringArray())
    {
        // State button
        isOn.setButtonText("STATE");
        isOn.setClickingTogglesState(false);
        isOn.setInterceptsMouseClicks(false, false);

        isOn.setComponentID(p.PID_ACTIVE);

        isOnLabel.setText("reverb", juce::NotificationType::dontSendNotification);
        isOnLabel.setJustificationType(juce::Justification::topLeft);
        isOnLabel.attachToComponent(&isOn, false);

        isOnAttachment.reset(new ButtonAttachment(p.parameters,
                                                  isOn.getComponentID(),
                                                  isOn));

        // IR file box
        juce::String currentIR = p.parameters.state
                                             .getChildWithName(p.PID_IR_FILE_CHOICE)
                                             .getProperty("value");

        irChoice.setButtonText(currentIR);

        irChoice.setComponentID(p.PID_IR_FILE_CHOICE);

        irChoiceLabel.setText("impulse response", juce::NotificationType::dontSendNotification);
        irChoiceLabel.setJustificationType(juce::Justification::topLeft);
        irChoiceLabel.attachToComponent(&irChoice, false);

        // Sample rate box
        auto sampleRateTemp = std::to_string(p.getSampleRate() / 1000);

        sampleRate.setButtonText(sampleRateTemp.substr(0, 4) + " kHz");
        sampleRate.setClickingTogglesState(false);
        sampleRate.setInterceptsMouseClicks(false, false);

        sampleRateLabel.setText("sample rate", juce::NotificationType::dontSendNotification);
        sampleRateLabel.setJustificationType(juce::Justification::topLeft);
        sampleRateLabel.attachToComponent(&sampleRate, false);

        // TODO: Update sample rate if it changes in DAW (currently set only on construction)

        // Show all elements
        addAndMakeVisible(isOn);
        addAndMakeVisible(irChoice);
        addAndMakeVisible(sampleRate);
    }

    //==============================================================================
    void reverb::UIHeaderBlock::paint(juce::Graphics& g)
    {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

        g.setColour(juce::Colours::white);
        g.setFont(15.0f);
    }

    //==============================================================================
    /**
    * @brief Manages the layout of UIHeaderBlock when the block is resized
    *
    * This function defines all the relative positioning of the various UIHeaderBlock
    * elements.
    */
    void reverb::UIHeaderBlock::resized()
    {
        juce::Rectangle<int> bounds(getLocalBounds());

        int boundsWidth = bounds.getWidth();
        int boundsHeight = bounds.getHeight();

        int padding = (int)std::ceil(AudioProcessorEditor::PADDING_REL * boundsHeight);
        int labelHeight = 20;

        // Draw frame
        // TODO: Make this look nice
        /*auto frameBounds = bounds.reduced(padding);
        frame.setBounds(frameBounds);

        bounds.reduce(padding, padding);*/

        // Distribute child elements in columns
        std::vector<juce::Rectangle<int>> cells(3);
        std::vector<double> cellWidths = { 0.15, 0.60, 0.25 };

        for (int i = 0; i < 3; ++i)
        {
            cells[i] = bounds.removeFromLeft((int)std::round(cellWidths[i] * boundsWidth));

            cells[i].removeFromTop(labelHeight);
            cells[i].reduce(padding, padding);
        }

        // Distribute elements in columns
        isOn.setBounds(cells[0]);
        irChoice.setBounds(cells[1]);
        sampleRate.setBounds(cells[2]);
    }

}