/*
  ==============================================================================

    UIFilterBlock.h

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "PluginProcessor.h"
#include "UIBlock.h"

namespace reverb {

    //==============================================================================
    class UIFilterBlock : public UIBlock
    {
    public:
        //==============================================================================
        UIFilterBlock(AudioProcessor& processor, int index,
            const juce::String& displayedName = juce::String());

        //==============================================================================
        using Ptr = std::unique_ptr<UIFilterBlock>;

        //==============================================================================
        void paint(juce::Graphics&) override;
        void resized() override;

        //==============================================================================
        juce::Slider freq;
        juce::Slider q;
        juce::Slider gain;

    protected:
        //==============================================================================
        using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

        //==============================================================================
        juce::Label freqLabel;
        std::unique_ptr<SliderAttachment> freqAttachment;

        juce::Label qLabel;
        std::unique_ptr<SliderAttachment> qAttachment;

        juce::Label gainLabel;
        std::unique_ptr<SliderAttachment> gainAttachment;

    private:
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UIFilterBlock)
    };

}

