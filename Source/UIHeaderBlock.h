/*
  ==============================================================================

    GUIHeaderBlock.h

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "PluginProcessor.h"
#include "UIBlock.h"

namespace reverb
{

    //==============================================================================
    class UIHeaderBlock : public UIBlock
    {
    public:
        //==============================================================================
        UIHeaderBlock(AudioProcessor& processor);

        //==============================================================================
        using Ptr = std::unique_ptr<UIHeaderBlock>;

        //==============================================================================
        void paint(juce::Graphics&) override;
        void resized() override;

        //==============================================================================
        juce::TextButton isOn;
        juce::TextButton irChoice;
        juce::TextButton sampleRate;

        juce::AudioProcessorValueTreeState& parameters;
        juce::StringArray previousSelectedIRs;

    protected:
        //==============================================================================
        using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

        //==============================================================================
        juce::Label isOnLabel;
        std::unique_ptr<ButtonAttachment> isOnAttachment;

        juce::Label irChoiceLabel;

        juce::Label sampleRateLabel;

    private:
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UIHeaderBlock)
    };

}
