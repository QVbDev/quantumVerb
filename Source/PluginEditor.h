/*
  ==============================================================================
  
    PluginEditor.cpp

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "UIFilterBlock.h"
#include "UIHeaderBlock.h"
#include "UIReverbBlock.h"
#include "LookAndFeel.h"
#include "PluginProcessor.h"

namespace reverb
{

	//==============================================================================
	/**
    * Implements the plugins UI. This includes window configuration, placement of UI elements and handlers for 
    * various events such as slider interaction.
	*/
	class AudioProcessorEditor : public juce::AudioProcessorEditor
                               , public juce::Button::Listener 
                               , public juce::Slider::Listener
                               , public juce::TextEditor::Listener
	{
	public:
		AudioProcessorEditor(AudioProcessor&);
		~AudioProcessorEditor();

		//==============================================================================
		void paint(juce::Graphics&) override;
		void resized() override;

        //==============================================================================
        void buttonClicked(juce::Button * button) override;
        /*void loadIR(int num);
        void handleMenuResult(int result);
        void menuCallback(int result);*/
        void sliderValueChanged(juce::Slider *changedSlider) override;

        //==============================================================================
        static constexpr double PADDING_REL = 0.03;   // 3% padding

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
        UIReverbBlock reverbBlock;

        //==============================================================================
        UIFilterBlock lowShelfFilterBlock;
        UIFilterBlock peakLowFilterBlock;
        UIFilterBlock peakHighFilterBlock;
        UIFilterBlock highShelfFilterBlock;

    private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessorEditor)
	};

}