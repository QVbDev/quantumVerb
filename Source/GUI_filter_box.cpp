#include "GUI_filter_box.h"

namespace reverb {

    GUI_filter_box::GUI_filter_box()
    { 
        paramQ.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        paramf.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        paramA.setSliderStyle(juce::Slider::RotaryVerticalDrag);

        // text display pos, readonly, width, height
        paramQ.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
        paramf.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
        paramA.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);

        paramQ.setRange(0, 100, 1);
        paramf.setRange(0, 100, 1);
        paramA.setRange(0, 100, 1);

        paramQ.setComponentID("paramQ");
        paramf.setComponentID("paramf");
        paramA.setComponentID("paramA");

        QLabel.setText("Q", juce::NotificationType::dontSendNotification);
        QLabel.setJustificationType(juce::Justification::centredBottom);
        fLabel.setText("freq", juce::NotificationType::dontSendNotification);
        fLabel.setJustificationType(juce::Justification::centredBottom);
        ALabel.setText("amplitude", juce::NotificationType::dontSendNotification);
        ALabel.setJustificationType(juce::Justification::centredBottom);

        QLabel.attachToComponent(&paramQ, false);
        fLabel .attachToComponent(&paramf, false);
        ALabel.attachToComponent(&paramA, false);

        addAndMakeVisible(paramQ);
        addAndMakeVisible(paramf);
        addAndMakeVisible(paramA);        
    }


    GUI_filter_box::~GUI_filter_box()
    {
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

        paramA.setBounds(currentBox.removeFromLeft(currentBox.getWidth() / 2));
        paramf.setBounds(currentBox);
        paramQ.setBounds(footer);

    }

    // allows acces to sliders that were interacted with
    // [in] : pointer to slider that was interacted with
    // [out] : returns pointer to object that matches the object interacted with
    juce::Slider * GUI_filter_box::getSlider(juce::Slider * sliderToFind)
    {
        if (sliderToFind == &paramQ) {
            return &paramQ;
        }
        else if (sliderToFind == &paramf) {
            return &paramf;
        }
        else if (sliderToFind == &paramA) {
            return &paramA;
        }
        else {
            return  nullptr;
        };
    }

    // adds listeners to passed object
    // [in]: pointer to object to add as listener
    void GUI_filter_box::addListener(juce::Slider::Listener * pluginEditor)
    {
        paramQ.addListener(pluginEditor);
        paramf.addListener(pluginEditor);
        paramA.addListener(pluginEditor);
    }
}
