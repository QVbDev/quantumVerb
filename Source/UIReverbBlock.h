/*
  ==============================================================================

    UIReverbBlock.h

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "PluginProcessor.h"
#include "UIBlock.h"

#include <vector>


namespace reverb {

    //==============================================================================
    class UIReverbBlock : public UIBlock
    {
    public:
        //==============================================================================
        UIReverbBlock(AudioProcessor& processor);

        //==============================================================================
        using Ptr = std::unique_ptr<UIReverbBlock>;

        //==============================================================================
        void paint(juce::Graphics&) override;
        void resized() override;

        //==============================================================================
        juce::Slider irLength;
        juce::Slider preDelay;
        juce::Slider irGain;
        juce::Slider outGain;
        juce::Slider wetRatio;

    protected:
        //==============================================================================
        using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

        //==============================================================================
        juce::Label irLengthLabel;
        std::unique_ptr<SliderAttachment> irLengthAttachment;

        juce::Label preDelayLabel;
        std::unique_ptr<SliderAttachment> preDelayAttachment;

        juce::Label irGainLabel;
        std::unique_ptr<SliderAttachment> irGainAttachment;

        juce::Label outGainLabel;
        std::unique_ptr<SliderAttachment> outGainAttachment;

        juce::Label wetRatioLabel;
        std::unique_ptr<SliderAttachment> wetRatioAttachment;

    private:
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UIReverbBlock)
    };

}

