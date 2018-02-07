#include "GUI_filter_box.h"

namespace reverb {

    GUI_filter_box::GUI_filter_box()
    {
        

        paramQ.setSliderStyle(juce::Slider::Rotary);
        paramf.setSliderStyle(juce::Slider::Rotary);
        paramA.setSliderStyle(juce::Slider::Rotary);

        // text display pos, readonly, width, height
        paramQ.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
        paramf.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
        paramA.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);

        paramQ.setRange(0, 100, 1);
        paramf.setRange(0, 100, 1);
        paramA.setRange(0, 100, 1);

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
        juce::Rectangle<int> footer(currentBox.removeFromBottom(currentBox.getHeight() / 2));

        paramA.setBounds(currentBox.removeFromLeft(currentBox.getWidth() / 2));
        paramf.setBounds(currentBox);
        paramQ.setBounds(footer);

    }
}
