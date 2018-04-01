/*
  ==============================================================================
  
    PluginEditor.cpp

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "UIFilterBlock.h"
#include "UIGraphBlock.h"
#include "UIHeaderBlock.h"
#include "UIReverbBlock.h"
#include "LookAndFeel.h"
#include "PluginProcessor.h"

namespace reverb
{

    //==============================================================================
    /**
    * Implements the plugins UI. This includes window configuration, placement of UI elements
    * and handlers for various events such as slider interaction.
    */
    class AudioProcessorEditor : public juce::AudioProcessorEditor,
                                 public juce::Button::Listener
    {
    public:
        AudioProcessorEditor(AudioProcessor&);
        ~AudioProcessorEditor();

        //==============================================================================
        void paint(juce::Graphics&) override;
        void resized() override;

        //==============================================================================
        void buttonClicked(juce::Button* button) override;
        static void menuCallback(int result, UIHeaderBlock* headerBlock);

        //==============================================================================
        static constexpr double PADDING_REL = 0.02;   // 2% padding

    protected:
        //==============================================================================
        AudioProcessor& processor;
        juce::AudioProcessorValueTreeState& parameters;

        //==============================================================================
        LookAndFeel lookAndFeel;

        //==============================================================================
        using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
        using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

        //==============================================================================
        UIHeaderBlock headerBlock;

        //==============================================================================
        UIGraphBlock graphBlock;

        //==============================================================================
        UIReverbBlock reverbBlock;

        //==============================================================================
        UIFilterBlock lowShelfFilterBlock;
        UIFilterBlock peakLowFilterBlock;
        UIFilterBlock peakHighFilterBlock;
        UIFilterBlock highShelfFilterBlock;

        std::array<UIFilterBlock*, 4> filterBlocks;
        std::array<juce::String, 4> filterBlockNames;

    private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessorEditor)
	};

}