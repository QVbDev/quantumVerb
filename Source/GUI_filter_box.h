#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


namespace reverb {

    class GUI_filter_box : public juce::Component
    {
    public:
        GUI_filter_box();
        ~GUI_filter_box();

        void paint(juce::Graphics&) override;
        void resized() override;

        juce::Slider* getSlider(juce::Slider* sliderToFind);
        void addListener(juce::Slider::Listener * pluginEditor);

        juce::Slider paramQ;
        juce::Label QLabel;
        juce::Slider paramf;
        juce::Label fLabel;
        juce::Slider paramA;
        juce::Label ALabel;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GUI_filter_box)
    };

}

