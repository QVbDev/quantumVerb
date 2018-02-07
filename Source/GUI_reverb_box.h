#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


namespace reverb {

    class GUI_reverb_box : public juce::Component
    {
    public:
        GUI_reverb_box();
        ~GUI_reverb_box();

        void paint(juce::Graphics&) override;
        void resized() override;
    private:

        juce::Slider lenIR;
        juce::Slider volIR;
        juce::Slider gainOut;
        juce::Slider dryWet;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GUI_reverb_box)
    };

}

