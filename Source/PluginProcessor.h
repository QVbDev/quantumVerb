/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

#include "IRBank.h"
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

		void processBlock(juce::AudioSampleBuffer& audio, juce::MidiBuffer&) override;

        void processBlockBypassed(juce::AudioSampleBuffer& audio, juce::MidiBuffer& midi) override;

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
        juce::AudioProcessorValueTreeState parameters;
        
        // Unique parameter IDs
        static constexpr const char * PID_ACTIVE             = "is_active";

        static constexpr const char * PID_IR_FILE_CHOICE     = "ir_file_choice";
        static constexpr const char * PID_IR_LENGTH          = "ir_length";

        static constexpr const char * PID_FILTER_PREFIX      = "filter";
        static constexpr const char * PID_FILTER_FREQ_SUFFIX = "_freq";
        static constexpr const char * PID_FILTER_Q_SUFFIX    = "_q";
        static constexpr const char * PID_FILTER_GAIN_SUFFIX = "_gain";
        
        static constexpr const char * PID_PREDELAY           = "predelay";
        static constexpr const char * PID_IR_GAIN            = "ir_gain";
        static constexpr const char * PID_WETRATIO           = "wetratio";
        static constexpr const char * PID_AUDIO_OUT_GAIN     = "audio_out_gain";

        //==============================================================================
        std::vector<IRPipeline::Ptr>   irPipelines;
        std::vector<MainPipeline::Ptr> mainPipelines;

        std::mutex updatingParams;


    protected:
        //==============================================================================
        void initParams();
        bool paramsInitialised = false;

        //==============================================================================
        static constexpr int NUM_BLOCKS_PER_UPDATE_PARAMS = 5;
        int64_t blocksProcessed = 0;

        void updateParams(double sampleRate);
        void updateParamsForChannel(int channelIdx, double sampleRate);

        //==============================================================================
        void processChannel(int channelIdx);

        AudioBlock audioChannels;

	private:
		//==============================================================================
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessor)
	};

}