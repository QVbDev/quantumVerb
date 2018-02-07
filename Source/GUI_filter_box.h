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
    private:

        juce::Slider paramQ;
        juce::Slider paramf;
        juce::Slider paramA;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GUI_filter_box)
    };

}

