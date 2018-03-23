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
        : UIBlock(3, 3, "Header", "Reverb Settings")
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

        irChoice.setComponentID(p.PID_IR_FILE_CHOICE);

        irChoice.addItem("Load custom file...", irChoice.getNumItems() + 1);
        irChoice.addSeparator();
        for (auto irFile : p.irBank.buffers) {
            irChoice.addItem(irFile.first, irChoice.getNumItems() + 1);
        }
        irChoice.setSelectedItemIndex(1);

        irChoiceLabel.setText("impulse response", juce::NotificationType::dontSendNotification);
        irChoiceLabel.setJustificationType(juce::Justification::topLeft);
        irChoiceLabel.attachToComponent(&irChoice, false);

        irChoiceAttachment.reset(new ComboBoxAttachment(p.parameters,
            irChoice.getComponentID(),
            irChoice));

        // TODO: Ensure IR file box tracks the IR choice parameter

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
    void reverb::UIHeaderBlock::resized()
    {
        juce::Rectangle<int> bounds(getLocalBounds());

        int boundsWidth = bounds.getWidth();
        int boundsHeight = bounds.getHeight();

        int padding = AudioProcessorEditor::PADDING_REL * boundsHeight;
        int labelHeight = 20;

        // Distribute child elements in columns
        std::vector<juce::Rectangle<int>> cells(3);
        std::vector<double> cellWidths = { 0.15, 0.60, 0.25 };

        for (int i = 0; i < 3; ++i)
        {
            cells[i] = bounds.removeFromLeft(cellWidths[i] * boundsWidth);

            cells[i].removeFromTop(labelHeight);
            cells[i].reduce(padding, padding);
        }

        // Distribute elements in columns
        isOn.setBounds(cells[0]);
        irChoice.setBounds(cells[1]);
        sampleRate.setBounds(cells[2]);
    }

    void reverb::UIHeaderBlock::addListener(juce::ComboBox::Listener* pluginEditor)
    {
        irChoice.addListener(pluginEditor);
    }

    juce::ComboBox* reverb::UIHeaderBlock::getComboBox(juce::ComboBox* clickedComboBox)
    {
        if (clickedComboBox == &irChoice) {
            return &irChoice;
        }
        else {
            return nullptr;
        }
    }

}