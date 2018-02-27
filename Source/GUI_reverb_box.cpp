#include "GUI_reverb_box.h"

namespace reverb {

    GUI_reverb_box::GUI_reverb_box()
    {
        

        lenIR.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        volIR.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        gainOut.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        dryWet.setSliderStyle(juce::Slider::RotaryVerticalDrag);

        // text display pos, readonly, width, height
        lenIR.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
        volIR.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
        gainOut.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
        dryWet.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);

        lenIR.setRange(0, 100, 1);
        volIR.setRange(0, 100, 1);
        gainOut.setRange(0, 100, 1);
        dryWet.setRange(0, 100, 1);

        lenIR.setComponentID("lenIR");
        volIR.setComponentID("volIR");
        gainOut.setComponentID("gainOut");
        dryWet.setComponentID("dryWet");

        lengthLabel.setText("IR length", juce::NotificationType::dontSendNotification);
        lengthLabel.setJustificationType(juce::Justification::centredBottom);
        volumeLabel.setText("IR volume", juce::NotificationType::dontSendNotification);
        volumeLabel.setJustificationType(juce::Justification::centredBottom);
        gainLabel.setText("out gain", juce::NotificationType::dontSendNotification);
        gainLabel.setJustificationType(juce::Justification::centredBottom);
        dryWetLabel.setText("mixer ratio", juce::NotificationType::dontSendNotification);
        dryWetLabel.setJustificationType(juce::Justification::centredBottom);

        lengthLabel.attachToComponent(&lenIR, false);
        volumeLabel.attachToComponent(&volIR, false);
        gainLabel.attachToComponent(&gainOut, false);
        dryWetLabel.attachToComponent(&dryWet, false);


        addAndMakeVisible(lenIR);
        addAndMakeVisible(volIR);
        addAndMakeVisible(gainOut);
        addAndMakeVisible(dryWet);

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

        lenIR.setBounds(currentBox.removeFromTop(sliderHeight));
        volIR.setBounds(currentBox.removeFromTop(sliderHeight));
        gainOut.setBounds(currentBox.removeFromTop(sliderHeight));
        dryWet.setBounds(currentBox);

    }

    // allows acces to sliders that were interacted with
    // [in] : pointer to slider that was interacted with
    // [out] : returns pointer to object that matches the object interacted with
    juce::Slider* GUI_reverb_box::getSlider(juce::Slider * sliderToFind)
    {
        if (sliderToFind == &lenIR) {
            return &lenIR;
        }
        else if (sliderToFind == &volIR) {
            return &volIR;
        }
        else if (sliderToFind == &gainOut) {
            return &gainOut;
        }
        else if (sliderToFind == &dryWet) {
            return &dryWet;
        }
        else {
            return  nullptr;
        }
    }

    // adds listeners to passed object
    // [in]: pointer to object to add as listener
    void GUI_reverb_box::addListener(juce::Slider::Listener * pluginEditor)
    {
        lenIR.addListener(pluginEditor);
        volIR.addListener(pluginEditor);
        gainOut.addListener(pluginEditor);
        dryWet.addListener(pluginEditor);
    }
}
