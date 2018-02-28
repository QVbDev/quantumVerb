#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <vector>

#include "PluginProcessor.h"


namespace reverb {

    class GUI_reverb_box : public juce::Component
    {
    public:
        GUI_reverb_box(AudioProcessor& processor);
        ~GUI_reverb_box();

        void paint(juce::Graphics&) override;
        void resized() override;

    protected:
        using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    public:
        juce::Slider irLength;
        juce::Label irLengthLabel;
        std::unique_ptr<SliderAttachment> irLengthAttachment;

        juce::Slider irGain;
        juce::Label irGainLabel;
        std::unique_ptr<SliderAttachment> irGainAttachment;

        juce::Slider outGain;
        juce::Label outGainLabel;
        std::unique_ptr<SliderAttachment> outGainAttachment;

        juce::Slider wetRatio;
        juce::Label wetRatioLabel;
        std::unique_ptr<SliderAttachment> wetRatioAttachment;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GUI_reverb_box)
    };

}

