/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

#include "IRPipeline.h"
#include "MainPipeline.h"

#include <map>
#include <mutex>
#include <vector>

namespace reverb
{

	//==============================================================================
	/**
     * Main reverb audio processor class. Contains all processing steps and provides a user-
     * facing editor for parameter adjustment.
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

		void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer& = juce::MidiBuffer()) override;

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
        std::mutex lock;

    protected:
        //==============================================================================
        std::vector<IRPipeline::Ptr>   irPipelines;
        std::vector<MainPipeline::Ptr> mainPipelines;

        std::vector<juce::AudioSampleBuffer> irChannels;
        std::vector<juce::AudioSampleBuffer> audioChannels;

	private:
		//==============================================================================
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessor)
	};

}