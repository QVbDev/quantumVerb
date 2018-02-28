#include "GUI_filter_box.h"

namespace reverb {

    GUI_filter_box::GUI_filter_box(AudioProcessor& processor, int index)
    {
        // Sliders
        freq.setSliderStyle(juce::Slider::Rotary);
        q.setSliderStyle(juce::Slider::Rotary);
        gain.setSliderStyle(juce::Slider::Rotary);

        freq.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        q.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        gain.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

        juce::String filterIDPrefix = processor.PID_FILTER_PREFIX + std::to_string(index);

        freq.setComponentID(filterIDPrefix + processor.PID_FILTER_FREQ_SUFFIX);
        q.setComponentID(filterIDPrefix + processor.PID_FILTER_Q_SUFFIX);
        gain.setComponentID(filterIDPrefix + processor.PID_FILTER_GAIN_SUFFIX);

        // Labels
        qLabel.setText("Q", juce::NotificationType::dontSendNotification);
        qLabel.setJustificationType(juce::Justification::centredBottom);

        freqLabel.setText("freq", juce::NotificationType::dontSendNotification);
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
    }

    void reverb::GUI_filter_box::paint(juce::Graphics& g)
    {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

        g.setColour(juce::Colours::white);
        g.setFont(15.0f);
    }

    void reverb::GUI_filter_box::resized()
    {
        juce::Rectangle<int> currentBox(getLocalBounds());
        auto emptyHeader = currentBox.removeFromTop(currentBox.getHeight()*0.1);
        juce::Rectangle<int> footer(currentBox.removeFromBottom(currentBox.getHeight() / 2));

        q.setBounds(footer);
        gain.setBounds(currentBox.removeFromLeft(currentBox.getWidth() / 2));
        freq.setBounds(currentBox);
    }
}
