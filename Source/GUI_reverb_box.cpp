#include "GUI_reverb_box.h"

namespace reverb {

    GUI_reverb_box::GUI_reverb_box(AudioProcessor& processor)
    {
        // Sliders
        irLength.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        irGain.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        outGain.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        wetRatio.setSliderStyle(juce::Slider::RotaryVerticalDrag);

        irLength.setComponentID(processor.PID_IR_LENGTH);
        irGain.setComponentID(processor.PID_IR_GAIN);
        outGain.setComponentID(processor.PID_AUDIO_OUT_GAIN);
        wetRatio.setComponentID(processor.PID_WETRATIO);

        // Labels
        irLengthLabel.setText("Reverb length", juce::NotificationType::dontSendNotification);
        irLengthLabel.setJustificationType(juce::Justification::centredBottom);

        irGainLabel.setText("Reverb volume", juce::NotificationType::dontSendNotification);
        irGainLabel.setJustificationType(juce::Justification::centredBottom);

        outGainLabel.setText("Output volume", juce::NotificationType::dontSendNotification);
        outGainLabel.setJustificationType(juce::Justification::centredBottom);

        wetRatioLabel.setText("Wet ratio", juce::NotificationType::dontSendNotification);
        wetRatioLabel.setJustificationType(juce::Justification::centredBottom);

        irLengthLabel.attachToComponent(&irLength, false);
        irGainLabel.attachToComponent(&irGain, false);
        outGainLabel.attachToComponent(&outGain, false);
        wetRatioLabel.attachToComponent(&wetRatio, false);

        // Attachments
        irLengthAttachment.reset(new SliderAttachment(processor.parameters,
                                                      irLength.getComponentID(),
                                                      irLength));

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
        addAndMakeVisible(irGain);
        addAndMakeVisible(outGain);
        addAndMakeVisible(wetRatio);
    }


    GUI_reverb_box::~GUI_reverb_box()
    {
    }

    void reverb::GUI_reverb_box::paint(juce::Graphics& g)
    {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

        g.setColour(juce::Colours::white);
        g.setFont(15.0f);
    }

    void reverb::GUI_reverb_box::resized()
    {
        juce::Rectangle<int> currentBox(getLocalBounds());
        auto emptyHeader = currentBox.removeFromTop(currentBox.getHeight()*0.1);
        int sliderHeight = currentBox.getHeight() / 4;

        irLength.setBounds(currentBox.removeFromTop(sliderHeight));
        irGain.setBounds(currentBox.removeFromTop(sliderHeight));
        outGain.setBounds(currentBox.removeFromTop(sliderHeight));
        wetRatio.setBounds(currentBox);

    }
}
