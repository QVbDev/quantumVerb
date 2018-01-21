/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "IRPipeline.h"
#include "MainPipeline.h"

#include <mutex>

namespace reverb
{

	//==============================================================================
	/**
    * TODO: Description
	*/
	class AudioProcessor : public juce::AudioProcessor
	{
	public:
		//==============================================================================
		AudioProcessor();
		~AudioProcessor();

		//==============================================================================
		void prepareToPlay(double sampleRate, int samplesPerBlock) override;
		void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
		bool isBusesLayoutSupported(const juce::BusesLayout& layouts) const override;
#endif

		void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer&) override;

		//==============================================================================
		juce::AudioProcessorEditor* createEditor() override;
		bool hasEditor() const override;

		//==============================================================================
		const juce::String getName() const override;

		bool acceptsMidi() const override;
		bool producesMidi() const override;
		bool isMidiEffect() const override;
		double getTailLengthSeconds() const override;

		//==============================================================================
		int getNumPrograms() override;
		int getCurrentProgram() override;
		void setCurrentProgram(int index) override;
		const juce::String getProgramName(int index) override;
		void changeProgramName(int index, const juce::String& newName) override;

		//==============================================================================
		void getStateInformation(juce::MemoryBlock& destData) override;
		void setStateInformation(const void* data, int sizeInBytes) override;

        //==============================================================================
        IRPipeline::Ptr irPipeline;
        MainPipeline::Ptr mainPipeline;

        std::mutex lock;

	private:
		//==============================================================================
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessor)
	};

}