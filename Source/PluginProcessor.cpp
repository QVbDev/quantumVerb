/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Util.h"

namespace reverb
{

	//==============================================================================
    /**
     * @brief Constructs a reverb audio processor & initialises its inner pipelines
     */
	AudioProcessor::AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
		: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
			.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
			.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
		)
#endif
	{
        try
        {
            irPipeline = std::make_shared<IRPipeline>(this);
        }
        catch (const std::exception& e)
        {
            logger.dualPrint(Logger::Level::Fatal, e.what());

            // TODO: Let user know about error through UI

            // TODO: Find a way to exit without crashing the current process so we don't kill the DAW
        }

        mainPipeline = std::make_shared<MainPipeline>(this);
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
	void AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
	{
		// Use this method as the place to do any pre-playback
		// initialisation that you need..
	}

	void AudioProcessor::releaseResources()
	{
		// When playback stops, you can use this as an opportunity to free up any
		// spare memory, etc.
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

        // Execute IR pipeline if necessary (i.e. first run or one of its parameters
        // was changed by the editor)
        if (irPipeline->mustExec)
        {
            juce::AudioSampleBuffer ir;

            try
            {
                irPipeline->exec(ir);
            }
            catch (const std::exception& e)
            {
                std::string errMsg = "Skipping IR pipeline due to exception:";
                            errMsg += e.what();

                logger.dualPrint(Logger::Level::Error, errMsg);

                // TODO: Let user know about error through UI (?)
            }

            mainPipeline->loadIR(std::move(ir));
            irPipeline->mustExec = false;
        }

        // Execute main pipeline
        mainPipeline->exec(buffer);
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

}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new reverb::AudioProcessor();
}