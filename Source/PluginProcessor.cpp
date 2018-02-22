/*
  ==============================================================================
  
    PluginProcessor.cpp

  ==============================================================================
*/

#include "PluginProcessor.h"

#include "IRBank.h"
#include "PluginEditor.h"
#include "Logger.h"

namespace reverb
{

	//==============================================================================
    /**
     * @brief Constructs a reverb audio processor & initialises its inner pipelines
     */
    AudioProcessor::AudioProcessor()
        : parameters(*this, nullptr)
#ifndef JucePlugin_PreferredChannelConfigurations
		, AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
			.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
			.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
		)
#endif
	{
        initParams();
	}

	AudioProcessor::~AudioProcessor()
	{
	}

	//==============================================================================
	const juce::String AudioProcessor::getName() const
	{
		return JucePlugin_Name;
	}

	bool AudioProcessor::acceptsMidi() const
	{
#if JucePlugin_WantsMidiInput
		return true;
#else
		return false;
#endif
	}

	bool AudioProcessor::producesMidi() const
	{
#if JucePlugin_ProducesMidiOutput
		return true;
#else
		return false;
#endif
	}

	bool AudioProcessor::isMidiEffect() const
	{
#if JucePlugin_IsMidiEffect
		return true;
#else
		return false;
#endif
	}

	double AudioProcessor::getTailLengthSeconds() const
	{
		return 0.0;
	}

	int AudioProcessor::getNumPrograms()
	{
		return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
					// so this should be at least 1, even if you're not really implementing programs.
	}

	int AudioProcessor::getCurrentProgram()
	{
		return 0;
	}

	void AudioProcessor::setCurrentProgram(int index)
	{
	}

	const juce::String AudioProcessor::getProgramName(int index)
	{
		return {};
	}

	void AudioProcessor::changeProgramName(int index, const juce::String& newName)
	{
	}

	//==============================================================================
    /**
     * @brief Prepare the processor before playback starts
     *
     * Called before playback starts, to let the processor prepare itself.
     * 
     * You can call getTotalNumInputChannels and getTotalNumOutputChannels or query
     * the busLayout member variable to find out the number of channels your
     * processBlock callback must process.
     * 
     * The maximumExpectedSamplesPerBlock value is a strong hint about the maximum
     * number of samples that will be provided in each block. You may want to use
     * this value to resize internal buffers. You should program defensively in case
     * a buggy host exceeds this value. The actual block sizes that the host uses
     * may be different each time the callback happens: completely variable block
     * sizes can be expected from some hosts.
     *
     * @param sampleRate [in]       Target sample rate (constant until playback stops)
     * @param samplesPerBlock [in]  Hint about max. expected samples in upcoming block
     */
	void AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
	{
        int numChannels = getTotalNumInputChannels();

        if (getTotalNumInputChannels() != getTotalNumOutputChannels())
        {
            std::string errMsg = "Mismatch between input (" +
                                 std::to_string(getTotalNumInputChannels()) + 
                                 ") and output (" +
                                 std::to_string(getTotalNumOutputChannels()) +
                                 ") channels. Forcing output to " +
                                 std::to_string(numChannels) +
                                 " channels.";

            logger.dualPrint(Logger::Level::Error, errMsg);
        }

        // Add/remove pipelines as needed to meet requeted number of channels
        for (int i = numChannels; i < irPipelines.size(); ++i)      irPipelines.pop_back();
        for (int i = irPipelines.size(); i < numChannels; ++i)      irPipelines.emplace_back(new IRPipeline(this, irBank, i));

        for (int i = numChannels; i < mainPipelines.size(); ++i)    mainPipelines.pop_back();
        for (int i = mainPipelines.size(); i < numChannels; ++i)    mainPipelines.emplace_back(new MainPipeline(this));

        // Update parameters across pipelines
        for (auto& pipeline : irPipelines)
        {
            pipeline->updateParams(parameters);
            pipeline->updateSampleRate(sampleRate);
        }

        for (auto& pipeline : mainPipelines)
        {
            pipeline->updateParams(parameters);
        }

        // Add empty buffers to meet channel count if necessary
        for (int i = irChannels.size(); i < numChannels; ++i)
        {
            irChannels.emplace_back();
        }

        // Clear existing audio buffers and add/remove buffers of appropriate size if necessary
        for (int i = numChannels; i < audioChannels.size(); ++i)
        {
            audioChannels.pop_back();
        }

        for (auto& channel : audioChannels)
        {
            channel.setSize(1, samplesPerBlock);
        }

        for (int i = audioChannels.size(); i < numChannels; ++i)
        {
            audioChannels.emplace_back(1, samplesPerBlock);
        }
    }

    /**
     * @brief Release resource when playback stops
     *
     * Used when playback stops as an opportunity to free up any spare memory, etc.
     */
	void AudioProcessor::releaseResources()
	{
        audioChannels.clear();
	}

#ifndef JucePlugin_PreferredChannelConfigurations
	bool AudioProcessor::isBusesLayoutSupported(const juce::BusesLayout& layouts) const
	{
#if JucePlugin_IsMidiEffect
		ignoreUnused(layouts);
		return true;
#else
		// This is the place where you check if the layout is supported.
		// In this template code we only support mono or stereo.
		if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
			&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
			return false;

		// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
		if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
			return false;
#endif

		return true;
#endif
	}
#endif

    /**
     * @brief Applies reverb effect on audio buffer using parameters given through the editor
     *
     * @param [in] buffer   Audio buffer containing samples to process
     * @param midiMessages  Unused
     */
	void AudioProcessor::processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&)
	{
        // Lock processor during use
        std::lock_guard<std::mutex> guard(lock);

        // Disable denormals for performance
		juce::ScopedNoDenormals noDenormals;

        // In case we have more outputs than inputs, this code clears any output
        // channels that didn't contain input data, (because these aren't
        // guaranteed to be empty - they may contain garbage).
        const int totalNumInputChannels = getTotalNumInputChannels();
        const int totalNumOutputChannels = getTotalNumOutputChannels();

        for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        {
            buffer.clear(i, 0, buffer.getNumSamples());
        }

        // We should have the right number of pipelines and channel buffers from
        // a previous call to prepareToPlay(), but just to be safe let's check
        // these values here.
        for (int i = irPipelines.size(); i < buffer.getNumChannels(); ++i)   irPipelines.emplace_back();
        for (int i = mainPipelines.size(); i < buffer.getNumChannels(); ++i) mainPipelines.emplace_back();

        for (int i = irChannels.size(); i < buffer.getNumChannels(); ++i)    irChannels.emplace_back();
        for (int i = audioChannels.size(); i < buffer.getNumChannels(); ++i) audioChannels.emplace_back();

        // Split input buffer into one buffer per channel
        for (int i = 0; i < buffer.getNumChannels(); ++i)
        {
            // Audio channel should have been prepared with the right size in
            // prepareToPlay(), but we should play it safe in case the DAW gave
            // us the wrong number of samples then.
            if (audioChannels[i].getNumSamples() != buffer.getNumSamples())
            {
                audioChannels[i].setSize(1, buffer.getNumSamples(), true);
            }

            memcpy(audioChannels[i].getWritePointer(0),
                   buffer.getReadPointer(i),
                   buffer.getNumSamples() * sizeof(buffer.getReadPointer(0)[0]));
        }

        // Execute IR pipeline (only runs if necessary, i.e. on first run or if one
        // of its parameters was changed by the editor)
        for (int i = 0; i < buffer.getNumChannels(); ++i)
        {
            if (irPipelines[i]->needsToRun())
            {
                try
                {
                    irPipelines[i]->exec(irChannels[i]);
                }
                catch (const std::exception& e)
                {
                    std::string errMsg = "Skipping IR pipeline for channel " +
                        std::to_string(i) +
                        " due to exception: "
                        + e.what();

                    logger.dualPrint(Logger::Level::Error, errMsg);
                }

                mainPipelines[i]->loadIR(std::move(irChannels[i]));
            }
        }

        // Execute main pipeline
        for (int i = 0; i < irPipelines.size(); ++i)
        {
            mainPipelines[i]->exec(audioChannels[i]);
        }

        // Merge channels into output buffer
        for (int i = 0; i < buffer.getNumChannels(); ++i)
        {
            // Safety check, we shouldn't enter this section under normal circumstances
            if (buffer.getNumSamples() < audioChannels[i].getNumSamples())
            {
                std::string errMsg = "Processed audio unexpectedly contains more samples than"
                                     "input buffer (input: " +
                                     std::to_string(buffer.getNumSamples()) +
                                     " samples, output: " +
                                     std::to_string(audioChannels[i].getNumSamples()) +
                                     " samples)";
                
                buffer.setSize(buffer.getNumChannels(),
                               audioChannels[i].getNumSamples(), true);

                logger.dualPrint(Logger::Level::Error, errMsg);
            }

            auto channelReadPtr = audioChannels[i].getReadPointer(0);

            memcpy(buffer.getWritePointer(i),
                   channelReadPtr,
                   audioChannels[i].getNumSamples() * sizeof(channelReadPtr[0]));
        }
	}

	//==============================================================================
	bool AudioProcessor::hasEditor() const
	{
		return true; // (change this to false if you choose to not supply an editor)
	}

	juce::AudioProcessorEditor* AudioProcessor::createEditor()
	{
		return new AudioProcessorEditor(*this);
	}

	//==============================================================================
	void AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
	{
        // TODO

		// You should use this method to store your parameters in the memory block.
		// You could do that either as raw data, or use the XML or ValueTree classes
		// as intermediaries to make it easy to save and load complex data.
	}

	void AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
	{
        // TODO

		// You should use this method to restore your parameters from this memory block,
		// whose contents will have been created by the getStateInformation() call.
	}

    //==============================================================================
    void AudioProcessor::initParams()
    {
        // Only build parameters once. This saves work and avoids memory leaks due to
        // allocating memory for the same parameter more than once.
        if (paramsInitialised)
        {
            return;
        }

        static const float gain15dB = juce::Decibels::decibelsToGain(15);

        /**
         * Low-shelf filter
         */
        parameters.createAndAddParameter( PID_FILTER_PREFIX + std::to_string(0) + PID_FILTER_FREQ_SUFFIX,
                                          "EQ: Low-shelf cut-off frequency", "Hz",
                                          juce::NormalisableRange<float>(0, 2e4),
                                          1e3,
                                          nullptr, nullptr );

        // TODO: Upper limit on Q factor?
        parameters.createAndAddParameter( PID_FILTER_PREFIX + std::to_string(0) + PID_FILTER_Q_SUFFIX,
                                          "EQ: Low-shelf Q factor", "",
                                          juce::NormalisableRange<float>(0.7, 1e4),
                                          0.707,
                                          nullptr, nullptr );
        
        parameters.createAndAddParameter( PID_FILTER_PREFIX + std::to_string(0) + PID_FILTER_GAIN_SUFFIX,
                                          "EQ: Low-shelf gain factor", "1 = no change",
                                          juce::NormalisableRange<float>(0, gain15dB),
                                          0.707,
                                          nullptr, nullptr );

        
        /**
         * Peak filter 1
         */
        // TODO: Vary filter parameters by type (Alex?)
        parameters.createAndAddParameter( PID_FILTER_PREFIX + std::to_string(1) + PID_FILTER_FREQ_SUFFIX,
                                          "EQ: Peak filter 1 cut-off frequency", "Hz",
                                          juce::NormalisableRange<float>(0, 2e4),
                                          1e3,
                                          nullptr, nullptr );

        // TODO: Upper limit on Q factor?
        // TODO: Vary filter parameters by type (Alex?)
        parameters.createAndAddParameter( PID_FILTER_PREFIX + std::to_string(1) + PID_FILTER_Q_SUFFIX,
                                          "EQ: Peak filter 1 Q factor", "",
                                          juce::NormalisableRange<float>(0.7, 1e4),
                                          0.707,
                                          nullptr, nullptr );

        // TODO: Vary filter parameters by type (Alex?)
        parameters.createAndAddParameter( PID_FILTER_PREFIX + std::to_string(1) + PID_FILTER_GAIN_SUFFIX,
                                          "EQ: Peak filter 1 gain factor", "1 = no change",
                                          juce::NormalisableRange<float>(0, gain15dB),
                                          0.707,
                                          nullptr, nullptr );

        
        /**
         * Peak filter 2
         */
        // TODO: Vary filter parameters by type (Alex?)
        parameters.createAndAddParameter( PID_FILTER_PREFIX + std::to_string(2) + PID_FILTER_FREQ_SUFFIX,
                                          "EQ: Peak filter 2 cut-off frequency", "Hz",
                                          juce::NormalisableRange<float>(0, 2e4),
                                          1e3,
                                          nullptr, nullptr );

        // TODO: Upper limit on Q factor?
        // TODO: Vary filter parameters by type (Alex?)
        parameters.createAndAddParameter( PID_FILTER_PREFIX + std::to_string(2) + PID_FILTER_Q_SUFFIX,
                                          "EQ: Peak filter 2 Q factor", "",
                                          juce::NormalisableRange<float>(0.7, 1e4),
                                          0.707,
                                          nullptr, nullptr );

        // TODO: Vary filter parameters by type (Alex?)
        parameters.createAndAddParameter( PID_FILTER_PREFIX + std::to_string(2) + PID_FILTER_GAIN_SUFFIX,
                                          "EQ: Peak filter 2 gain factor", "1 = no change",
                                          juce::NormalisableRange<float>(0, gain15dB),
                                          0.707,
                                          nullptr, nullptr );


        /**
         * Low-shelf filter
         */
        // TODO: Vary filter parameters by type (Alex?)
        parameters.createAndAddParameter( PID_FILTER_PREFIX + std::to_string(3) + PID_FILTER_FREQ_SUFFIX,
                                          "EQ: High-shelf cut-off frequency", "Hz",
                                          juce::NormalisableRange<float>(0, 2e4),
                                          1e3,
                                          nullptr, nullptr );

        // TODO: Upper limit on Q factor?
        // TODO: Vary filter parameters by type (Alex?)
        parameters.createAndAddParameter( PID_FILTER_PREFIX + std::to_string(3) + PID_FILTER_Q_SUFFIX,
                                          "EQ: High-shelf Q factor", "",
                                          juce::NormalisableRange<float>(0.7, 1e4),
                                          0.707,
                                          nullptr, nullptr );

        // TODO: Vary filter parameters by type (Alex?)
        parameters.createAndAddParameter( PID_FILTER_PREFIX + std::to_string(3) + PID_FILTER_GAIN_SUFFIX,
                                          "EQ: High-shelf gain factor", "1 = no change",
                                          juce::NormalisableRange<float>(0, gain15dB),
                                          0.707,
                                          nullptr, nullptr );


        /**
         * IR gain
         */
        // TODO: Upper limit on IR gain?
        parameters.createAndAddParameter( PID_IR_GAIN,
                                          "Impulse response gain", "1 = no change",
                                          juce::NormalisableRange<float>(0, gain15dB),
                                          1,
                                          nullptr, nullptr );


        /**
         * Pre-delay
         */
        parameters.createAndAddParameter( PID_PREDELAY,
                                          "Pre-delay", "ms",
                                          juce::NormalisableRange<float>(0, 1000),
                                          0,
                                          nullptr, nullptr );


        /**
         * Dry/wet mixer
         */
        // TODO: Default wet ratio?
        parameters.createAndAddParameter( PID_WETRATIO,
                                          "Dry/wet ratio", "0 = all dry, 1 = all wet",
                                          juce::NormalisableRange<float>(0, 1),
                                          0.5,
                                          nullptr, nullptr );


        /**
         * Output gain
         */
        // TODO: Is range appropriate (i.e. do we want to allow boosting volume too)?
        parameters.createAndAddParameter( PID_AUDIO_OUT_GAIN,
                                          "Output gain", "1 = no change",
                                          juce::NormalisableRange<float>(0, gain15dB),
                                          1,
                                          nullptr, nullptr );


        parameters.state = juce::ValueTree(juce::Identifier(JucePlugin_Name));

        
        /**
         * Impulse responses
         * 
         * IR file choice is given by a string that may be one of the following:
         *     1) name of a valid audio resource in BinaryData.h
         *     2) full path to user-provided IR file
         */
        juce::ValueTree irFile(PID_IR_FILE_CHOICE);

        if (irBank.buffers.begin() != irBank.buffers.end())
        {
            const juce::String& firstBankedIR = irBank.buffers.begin()->first;
            irFile.setProperty("value", firstBankedIR, nullptr);
        }

        parameters.state.addChild(irFile, -1, nullptr);


        /**
         * END: Processor is responsible for deleting parameters on destruction
         */
        paramsInitialised = true;
    }

}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new reverb::AudioProcessor();
}