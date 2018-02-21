/*
  ==============================================================================
  
    PluginEditor.cpp

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "PluginProcessor.h"

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

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessorEditor)
	};

}