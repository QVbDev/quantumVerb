/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "PluginProcessor.h"

using namespace juce;

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

        TextButton activateButton;
        TextButton infoGene;
        TextButton sampleRate;
        TextButton graphBox;
        TextButton eqBox;
        TextButton gainMixTimeBox;
        //Rectangle<int> graphBox;
        Rectangle<int> rightControlBox;
        Rectangle<int> equalizerBox;
        
        Slider item2;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessorEditor)
	};

}