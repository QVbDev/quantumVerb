#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "PluginProcessor.h"

namespace reverb {

    class GUI_filter_box : public juce::Component
    {
    public:
        GUI_filter_box(AudioProcessor& processor, int index);

        void paint(juce::Graphics&) override;
        void resized() override;

    protected:
        using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    public:
        juce::Slider q;
        juce::Label qLabel;
        std::unique_ptr<SliderAttachment> qAttachment;

        juce::Slider freq;
        juce::Label freqLabel;
        std::unique_ptr<SliderAttachment> freqAttachment;

        juce::Slider gain;
        juce::Label gainLabel;
        std::unique_ptr<SliderAttachment> gainAttachment;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GUI_filter_box)
    };

}

