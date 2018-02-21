#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <vector>



namespace reverb {

    class GUI_reverb_box : public juce::Component
    {
    public:
        GUI_reverb_box();
        ~GUI_reverb_box();

        void paint(juce::Graphics&) override;
        void resized() override;

        juce::Slider* getSlider(juce::Slider* sliderToFind);
        void addListener(juce::Slider::Listener * pluginEditor);

    private:

        juce::Slider lenIR;
        juce::Label lengthLabel;
        juce::Slider volIR;
        juce::Label volumeLabel;
        juce::Slider gainOut;
        juce::Label gainLabel;
        juce::Slider dryWet;
        juce::Label dryWetLabel;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GUI_reverb_box)
    };

}

