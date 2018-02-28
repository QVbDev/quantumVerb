/*
  ==============================================================================
  
    PluginEditor.cpp

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "PluginProcessor.h"
#include "GUI_filter_box.h"
#include "GUI_reverb_box.h"

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

        void buttonClicked(juce::Button * button) override;
        /*void loadIR(int num);
        void handleMenuResult(int result);
        void menuCallback(int result);*/
        void sliderValueChanged(juce::Slider *changedSlider) override;

	private:
        //==============================================================================
		AudioProcessor& processor;
        juce::AudioProcessorValueTreeState& parameters;

        using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
        using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

        //==============================================================================
        GUI_filter_box lowShelf;
        GUI_filter_box peakingLow;
        GUI_filter_box peakingHigh;
        GUI_filter_box highShelf;

        GUI_reverb_box reverbParam;

        //==============================================================================
        juce::ToggleButton isOn;

        juce::TextButton genInfo;
        juce::TextEditor sampleRate;

        juce::Slider preDelay;
        juce::Label preDelayLabel;
        std::unique_ptr<SliderAttachment> preDelayAttachment;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessorEditor)
	};

}