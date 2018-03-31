/*
  ==============================================================================
  
    PluginProcessor.cpp

  ==============================================================================
*/

#include "PluginProcessor.h"

#include "IRBank.h"
#include "PluginEditor.h"
#include "Logger.h"

#ifdef WIN32
#include <windows.h>
#endif

#include <thread>

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
        std::lock_guard<std::mutex> lock(updatingParams);
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

	void AudioProcessor::setCurrentProgram(int)
	{
	}

	const juce::String AudioProcessor::getProgramName(int)
	{
		return {};
	}

	void AudioProcessor::changeProgramName(int, const juce::String&)
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
     * The samplesPerBlock value is a strong hint about the maximum
     * number of samples that will be provided in each block. You may want to use
     * this value to resize internal buffers. You should program defensively in case
     * a buggy host exceeds this value. The actual block sizes that the host uses
     * may be different each time the callback happens: completely variable block
     * sizes can be expected from some hosts.
     *
     * @param sampleRate [in]       Target sample rate (constant until playback stops)
     * @param samplesPerBlock [in]  Hint about max. expected samples in upcoming block
     */
    void AudioProcessor::prepareToPlay(double sampleRate, int)
    {
        size_t numChannels = getTotalNumInputChannels();

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
        for (size_t i = numChannels; i < irPipelines.size(); ++i)
        {
            irPipelines.pop_back();
        }

        for (size_t i = irPipelines.size(); i < numChannels; ++i)
        {
            irPipelines.emplace_back(new IRPipeline(this, (int)i));
        }

        for (size_t i = numChannels; i < mainPipelines.size(); ++i)
        {
            mainPipelines.pop_back();
        }

        for (size_t i = mainPipelines.size(); i < numChannels; ++i)
        {
            mainPipelines.emplace_back(new MainPipeline(this));
        }

        // Update parameters across pipelines
        updateParams(sampleRate);
    }

    /**
     * @brief Release resource when playback stops
     *
     * Used when playback stops as an opportunity to free up any spare memory, etc.
     */
	void AudioProcessor::releaseResources()
	{
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
	void AudioProcessor::processBlock(juce::AudioSampleBuffer& audio, juce::MidiBuffer&)
	{
#ifdef WIN32
        SetThreadPriority(GetCurrentThread(),
                          THREAD_PRIORITY_TIME_CRITICAL);
#endif

        // Reset the bypass detection parameter
        auto activeParam = parameters.getParameter(PID_ACTIVE);

        if (activeParam->getValue() < 0.5f)
        {
            activeParam->beginChangeGesture();
            activeParam->setValueNotifyingHost(1.0f);
            activeParam->endChangeGesture();
        }

        // Disable denormals for performance
		juce::ScopedNoDenormals noDenormals;

        // In case we have more outputs than inputs, this code clears any output
        // channels that didn't contain input data, (because these aren't
        // guaranteed to be empty - they may contain garbage).
        const int totalNumInputChannels = getTotalNumInputChannels();
        const int totalNumOutputChannels = getTotalNumOutputChannels();

        for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        {
            audio.clear(i, 0, audio.getNumSamples());
        }

        // We should have the right number of pipelines and channel buffers from
        // a previous call to prepareToPlay(), but just to be safe let's check
        // these values here.
        for (size_t i = irPipelines.size(); i < totalNumInputChannels; ++i)
        {
            irPipelines.emplace_back(new IRPipeline(this, (int)i));
        }

        for (size_t i = mainPipelines.size(); i < totalNumInputChannels; ++i)
        {
            mainPipelines.emplace_back(new MainPipeline(this));
        }

        // Associate audio block with input
        audioChannels = audio;

        // Update parameters asynchronously
        if (blocksProcessed % NUM_BLOCKS_PER_UPDATE_PARAMS)
        {
            std::unique_lock<std::mutex> lock(updatingParams, std::try_to_lock);

            // If parameters are already being updated, skip this and go straight
            // to processing.
            if (lock.owns_lock())
            {
                std::thread updateParamsThread(&AudioProcessor::updateParams,
                                               this, getSampleRate());

                lock.unlock();

                // updateParams() uses double buffering to update everything
                // asynchronously, so we can let it do its own thing.
                updateParamsThread.detach();
            }
        }

#if REVERB_MULTITHREADED > 0
        // Process each channel in its own thread
        std::vector<std::thread> channelThreads;

        for (int i = 0; i < totalNumInputChannels; ++i)
        {
            channelThreads.emplace_back(&AudioProcessor::processChannel, this, i);
        }

        // Wait for each thread to complete
        for (int i = 0; i < channelThreads.size(); ++i)
        {
            channelThreads[i].join();
        }
#else
        for (int i = 0; i < totalNumInputChannels; ++i)
        {
            processChannel(i);
        }
#endif

        blocksProcessed++;

#ifdef WIN32
        SetThreadPriority(GetCurrentThread(),
                          THREAD_PRIORITY_NORMAL);
#endif
	}

    /**
     * @brief Applies reverb effect on single channel using preconfigured pipelines
     *
     * @param [in] channelIdx   Index of channel to process
     */
    void AudioProcessor::processChannel(int channelIdx)
    {
#ifdef WIN32
        SetThreadPriority(GetCurrentThread(),
                          THREAD_PRIORITY_TIME_CRITICAL);
#endif

        AudioBlock channelAudio = audioChannels.getSingleChannelBlock(channelIdx);
        mainPipelines[channelIdx]->exec(channelAudio);

#ifdef WIN32
        SetThreadPriority(GetCurrentThread(),
                          THREAD_PRIORITY_NORMAL);
#endif
    }

    //==============================================================================
    /**
     * @brief Update parameters across all channels
     *
     * Update parameters in all IRPipeline and MainPipeline objects. If necessary,
     * reprocess IR and add to MainPipeline.
     *
     * MainPipeline is double-buffered to avoid interfering with processChannel().
     * IRPipeline is not used by any other methods, so it does not need protection.
     *
     * @param [in] sampleRate   Current sample rate
     */
    void AudioProcessor::updateParams(double sampleRate)
    {
#ifdef WIN32
        SetThreadPriority(GetCurrentThread(),
                          THREAD_MODE_BACKGROUND_BEGIN);
#endif

        // Obtain lock
        std::lock_guard<std::mutex> lock(updatingParams);

        // Check number of channels
        const int numChannels = (int)irPipelines.size();
        
        jassert(mainPipelines.size() == numChannels);

        // Process parameters for each channel
        for (int i = 0; i < numChannels; ++i)
        {
            updateParamsForChannel(i, sampleRate);
        }

#ifdef WIN32
        SetThreadPriority(GetCurrentThread(),
                          THREAD_MODE_BACKGROUND_END);
#endif
    }

    /**
     * @brief Update parameters for a given channel
     *
     * Update parameters in IRPipeline and Mainpipeline for given channel. If
     * necessary, reprocess IR using new parameters and copy to MainPipeline.
     *
     * MainPipeline is double-buffered to avoid interfering with processChannel().
     * IRPipeline is not used by any other methods, so it does not need protection.
     *
     * @param [in] channelIdx   Channel parameters should be updated for
     * @param [in] sampleRate   Current sample rate
     */
    void AudioProcessor::updateParamsForChannel(int channelIdx, double sampleRate)
    {
        auto& processorLock = getCallbackLock();

        auto& irPipeline = irPipelines[channelIdx];
        auto& mainPipeline = mainPipelines[channelIdx];

        AudioBlock irChannel;

        // Update IR parameters
        irPipeline->updateSampleRate(sampleRate);
        irPipeline->updateParams(parameters);

        // Reprocess IR if necessary
        bool updateIR = irPipeline->needsToRun();

        if (updateIR)
        {
            irChannel = irPipeline->exec();
        }

        // Update main parameters (critical section: mainPipeline is used by
        // processChannel)
        {
            juce::ScopedLock lock(processorLock);

            mainPipeline->updateParams(parameters);
            mainPipeline->updateSampleRate(sampleRate);

            if (updateIR)
            {
                mainPipeline->loadIR(irChannel);
            }
        }
    }

    //==============================================================================
    void AudioProcessor::processBlockBypassed(juce::AudioSampleBuffer& audio, juce::MidiBuffer& midi)
    {
        // Set the bypass detection parameter
        auto activeParam = parameters.getParameter(PID_ACTIVE);

        if (activeParam->getValue() > 0.5f)
        {
            activeParam->beginChangeGesture();
            activeParam->setValueNotifyingHost(0.0f);
            activeParam->endChangeGesture();
        }


        // Run JUCE's regular implementation
        this->juce::AudioProcessor::processBlockBypassed(audio, midi);
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
        std::unique_ptr<juce::XmlElement> xmlState(parameters.state.createXml());
        
        if (xmlState)
        {
            copyXmlToBinary(*xmlState, destData);
        }
        else
        {
            logger.dualPrint(Logger::Level::Error, "Failed to get state information");
        }
	}

	void AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
	{
        std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

        if (xmlState && xmlState->hasTagName(parameters.state.getType()))
        {
            parameters.state = juce::ValueTree::fromXml(*xmlState);
        }
        else
        {
            logger.dualPrint(Logger::Level::Error, "Failed to set state information");
        }
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

        static const float gain12dB = juce::Decibels::decibelsToGain<float>(15);
        static const float gain6dBneg = juce::Decibels::decibelsToGain<float>(-6);
        static const float gain24dBneg = juce::Decibels::decibelsToGain<float>(-24);
        static const float gain50dBneg = juce::Decibels::decibelsToGain<float>(-50);
        static const float gain15dBInv = 1.0f / gain12dB;

        auto fnGainTodBString = [](float gain) { return juce::String(juce::Decibels::gainToDecibels(gain), 2) + " dB"; };

        /**
         * Low-shelf filter
         */
        parameters.createAndAddParameter( PID_FILTER_PREFIX + std::to_string(0) + PID_FILTER_FREQ_SUFFIX,
                                          "EQ: Low-shelf cut-off frequency", "<16-520 Hz>",
                                          juce::Range<float>(16.0f, 520.0f),
                                          100.0f,
                                          nullptr, nullptr );

        // TODO: Upper limit on Q factor?
        parameters.createAndAddParameter( PID_FILTER_PREFIX + std::to_string(0) + PID_FILTER_Q_SUFFIX,
                                          "EQ: Low-shelf Q factor", "<0.71-1.41>",
                                          juce::Range<float>(0.71f, 1.41f),
                                          1.06f,
                                          nullptr, nullptr );
        
        parameters.createAndAddParameter( PID_FILTER_PREFIX + std::to_string(0) + PID_FILTER_GAIN_SUFFIX,
                                          "EQ: Low-shelf gain factor", "<" + juce::String(gain15dBInv) + " = no change>",
                                          juce::Range<float>(gain24dBneg, gain12dB),
                                          gain6dBneg,
                                          fnGainTodBString, nullptr );

        
        /**
         * Peak filter 1
         */
        parameters.createAndAddParameter( PID_FILTER_PREFIX + std::to_string(1) + PID_FILTER_FREQ_SUFFIX,
                                          "EQ: Peak filter 1 cut-off frequency", "<260-1600 Hz>",
                                          juce::NormalisableRange<float>(260.0f, 1040.0f),
                                          808.0f,
                                          nullptr, nullptr );

        parameters.createAndAddParameter( PID_FILTER_PREFIX + std::to_string(1) + PID_FILTER_Q_SUFFIX,
                                          "EQ: Peak filter 1 Q factor", "<0.26-6.5>",
                                          juce::NormalisableRange<float>(1.0f, 6.5f),
                                          3.38f,
                                          nullptr, nullptr );

        parameters.createAndAddParameter( PID_FILTER_PREFIX + std::to_string(1) + PID_FILTER_GAIN_SUFFIX,
                                          "EQ: Peak filter 1 gain factor", "<" + juce::String(gain15dBInv) + " = no change>",
                                          juce::NormalisableRange<float>(gain24dBneg, gain12dB),
                                          gain6dBneg,
                                          fnGainTodBString, nullptr );

        
        /**
         * Peak filter 2
         */
        parameters.createAndAddParameter( PID_FILTER_PREFIX + std::to_string(2) + PID_FILTER_FREQ_SUFFIX,
                                          "EQ: Peak filter 2 cut-off frequency", "<4000-16000 Hz>",
                                          juce::NormalisableRange<float>(4000.0f, 16000.0f),
                                          8000.0f,
                                          nullptr, nullptr );

        parameters.createAndAddParameter( PID_FILTER_PREFIX + std::to_string(2) + PID_FILTER_Q_SUFFIX,
                                          "EQ: Peak filter 2 Q factor", "<1.0-6.50>",
                                          juce::NormalisableRange<float>(1.0f, 6.50f),
                                          3.38f,
                                          nullptr, nullptr );

        parameters.createAndAddParameter( PID_FILTER_PREFIX + std::to_string(2) + PID_FILTER_GAIN_SUFFIX,
                                          "EQ: Peak filter 2 gain factor", "<" + juce::String(gain15dBInv) + " = no change>",
                                          juce::NormalisableRange<float>(gain24dBneg, gain12dB),
                                          gain6dBneg,
                                          fnGainTodBString, nullptr );


        /**
         * High-shelf filter
         */
        parameters.createAndAddParameter( PID_FILTER_PREFIX + std::to_string(3) + PID_FILTER_FREQ_SUFFIX,
                                          "EQ: High-shelf cut-off frequency", "<8000-21000 Hz>",
                                          juce::NormalisableRange<float>(8000.0f, 21000.0f),
                                          10000.0f,
                                          nullptr, nullptr );

        parameters.createAndAddParameter( PID_FILTER_PREFIX + std::to_string(3) + PID_FILTER_Q_SUFFIX,
                                          "EQ: High-shelf Q factor", "<0.71-1.41>",
                                          juce::NormalisableRange<float>(0.71f, 1.41f),
                                          1.06f,
                                          nullptr, nullptr );

        parameters.createAndAddParameter( PID_FILTER_PREFIX + std::to_string(3) + PID_FILTER_GAIN_SUFFIX,
                                          "EQ: High-shelf gain factor", "<" + juce::String(gain15dBInv) + " = no change>",
                                          juce::NormalisableRange<float>(gain24dBneg, gain12dB),
                                          gain6dBneg,
                                          fnGainTodBString, nullptr );


        /**
         * IR gain
         */
        parameters.createAndAddParameter( PID_IR_GAIN,
                                          "Impulse response gain", "<" + juce::String(1.0) + " = no change>",
                                          juce::NormalisableRange<float>(0.0f, 1.0f),
                                          0.5f,
                                          fnGainTodBString, nullptr );

        /**
         * IR length
         */
        parameters.createAndAddParameter( PID_IR_LENGTH,
                                          "Reverb length", "<s>",
                                          juce::NormalisableRange<float>(0.1f, 5.0f),
                                          3.0f,
                                          nullptr, nullptr);

        /**
         * Pre-delay
         */
        parameters.createAndAddParameter( PID_PREDELAY,
                                          "Pre-delay", "<0-1000 ms>",
                                          juce::NormalisableRange<float>(0.0f, 1000.0f),
                                          0.0f,
                                          nullptr, nullptr );


        /**
         * Dry/wet mixer
         */
        parameters.createAndAddParameter( PID_WETRATIO,
                                          "Dry/wet ratio", "<0 = dry, 1 = wet>",
                                          juce::NormalisableRange<float>(0.0f, 1.0f),
                                          0.5f,
                                          nullptr, nullptr );


        /**
         * Output gain
         */
        parameters.createAndAddParameter( PID_AUDIO_OUT_GAIN,
                                          "Output gain", "<" + juce::String(gain15dBInv) + "= no change>",
                                          juce::NormalisableRange<float>(gain50dBneg, 1.0f),
                                          1.0f,
                                          fnGainTodBString, nullptr );


        /**
        * Meta: Bypass detection flag
        */
        parameters.createAndAddParameter( PID_ACTIVE, "Active", "<y/n>",
                                          juce::NormalisableRange<float>(0.0f, 1.0f),
                                          1.0f,
                                          nullptr, nullptr,
                                          false, true, true );


        parameters.state = juce::ValueTree(juce::Identifier(JucePlugin_Name));
        
        /**
         * Impulse responses
         * 
         * IR file choice is given by a string that may be one of the following:
         *     1) name of a valid audio resource in BinaryData.h
         *     2) full path to user-provided IR file
         */
        juce::ValueTree irFile(PID_IR_FILE_CHOICE);

        auto& irBank = IRBank::getInstance();
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