/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

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
		// This reference is provided as a quick way for your editor to
		// access the processor object that created it.
		AudioProcessor& processor;

        GUI_filter_box lowShelf;
        GUI_filter_box peakingLow;
        GUI_filter_box peakingHigh;
        GUI_filter_box highShelf;

        GUI_reverb_box reverbParam;

        juce::ToggleButton isOn;

        juce::TextButton genInfo;
        juce::TextEditor sampleRate;

        juce::Slider preDelay;
        juce::Label preDelayLabel;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessorEditor)
	};

}