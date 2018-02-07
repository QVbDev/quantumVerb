#include "GUI_reverb_box.h"

namespace reverb {

    GUI_reverb_box::GUI_reverb_box()
    {
        

        lenIR.setSliderStyle(juce::Slider::Rotary);
        volIR.setSliderStyle(juce::Slider::Rotary);
        gainOut.setSliderStyle(juce::Slider::Rotary);
        dryWet.setSliderStyle(juce::Slider::Rotary);

        // text display pos, readonly, width, height
        lenIR.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
        volIR.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
        gainOut.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
        dryWet.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);

        lenIR.setRange(0, 100, 1);
        volIR.setRange(0, 100, 1);
        gainOut.setRange(0, 100, 1);
        dryWet.setRange(0, 100, 1);

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
        int sliderHeight = currentBox.getHeight() / 4;

        lenIR.setBounds(currentBox.removeFromTop(sliderHeight));
        volIR.setBounds(currentBox.removeFromTop(sliderHeight));
        gainOut.setBounds(currentBox.removeFromTop(sliderHeight));
        dryWet.setBounds(currentBox);

    }
}
