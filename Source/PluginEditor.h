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
    * TODO: Description
	*/
	class AudioProcessorEditor : public juce::AudioProcessorEditor 
	{
	public:
		AudioProcessorEditor(AudioProcessor&);
		~AudioProcessorEditor();

		//==============================================================================
		void paint(juce::Graphics&) override;
		void resized() override;

	private:
		// This reference is provided as a quick way for your editor to
		// access the processor object that created it.
		AudioProcessor& processor;

        /*// boxed sections buttons
        juce::TextButton activateButton;
        juce::TextButton infoGene;
        juce::TextButton sampleRate;
        juce::TextButton graphBox;
        juce::TextButton eqBox;
        juce::TextButton gainMixTimeBox;*/

        
        /*// slider comparison
        juce::Slider qfilter;
        juce::Slider qfilter1;
        juce::Slider qfilter2;
        juce::Slider qfilter3;
        juce::Slider qfilter4;
        juce::Slider qfilter5;*/

        GUI_filter_box lowShelf;
        GUI_filter_box peakingLow;
        GUI_filter_box peakingHigh;
        GUI_filter_box highShelf;

        GUI_reverb_box reverbParam;

        juce::ToggleButton isOn;

        juce::TextEditor genInfo;
        juce::TextEditor sampleRate;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessorEditor)
	};

}