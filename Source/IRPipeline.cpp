/*
  ==============================================================================

    IRPipeline.cpp

  ==============================================================================
*/

#include "IRPipeline.h"

#include "IRBank.h"
#include "Logger.h"
#include "PluginProcessor.h"

#include <algorithm>

namespace reverb
{

    //==============================================================================
    /**
     * @brief Constructs an IRPipeline object associated with an AudioProcessor
     *
     * Creates an IRPipeline and each of its steps. Also sets the default IR file path
     * and ensures it can be found.
     *
     * @param [in] processor    Pointer to main processor
     */
    IRPipeline::IRPipeline(juce::AudioProcessor * processor, const IRBank& irBank, int channelIdx)
        : Task(processor),
          irBank(irBank),
          channelIdx(channelIdx)
    {
        // Initialise pipeline steps
        filters[0] = std::make_shared<LowShelfFilter>(processor);
        filters[1] = std::make_shared<PeakFilter>(processor);
        filters[2] = std::make_shared<PeakFilter>(processor);
        filters[3] = std::make_shared<HighShelfFilter>(processor);

        timeStretch = std::make_shared<TimeStretch>(processor);
        gain = std::make_shared<Gain>(processor);
        preDelay = std::make_shared<PreDelay>(processor);

        // Get default IR from bank
        auto& firstIRName = irBank.buffers.begin()->first;

        loadIR(firstIRName);
    }

    //==============================================================================
    /**
     * @brief Read processor parameters and update child parameters as necessary
     * 
     * Update parameters based on current processor configuration. Set mustExec flag
     * if any parameters are changed in this process.
     *
     * @returns True if any parameters were changed, false otherwise.
     */
    bool IRPipeline::updateParams(const juce::AudioProcessorValueTreeState& params,
                                  const juce::String&)
    {
        bool changedConfig = false;

        // Update pipeline parameters
        auto paramIRChoice = params.state.getChildWithName(AudioProcessor::PID_IR_FILE_CHOICE);

        if (paramIRChoice.getProperty("value") != currentIR)
        {
            currentIR = paramIRChoice.getProperty("value");

            loadIR(currentIR.toStdString());

            changedConfig = true;
        }

        // Update child parameters
        for (int i = 0; i < filters.size(); ++i)
        {
            std::string filterId = AudioProcessor::PID_FILTER_PREFIX + std::to_string(i);
            changedConfig |= filters[i]->updateParams(params, filterId);
        }

        changedConfig |= gain->updateParams(params, AudioProcessor::PID_IR_GAIN);
        changedConfig |= preDelay->updateParams(params, AudioProcessor::PID_PREDELAY);

        // Update mustExec flag
        mustExec |= changedConfig;

        return changedConfig;
    }

    //==============================================================================
    /**
     * @brief Update sample rate for IR processing
     * 
     * Compares new sample rate with previous value. If different, sets mustExec to
     * true in order to re-run pipeline for new sample rate. Store new sample rate
     * value once complete.
     *
     * @returns True if sample rate changed, false otherwise.
     */
    bool IRPipeline::updateSampleRate(double sampleRate)
    {
        if (sampleRate != lastSampleRate)
        {
            mustExec = true;
            lastSampleRate = sampleRate;
        }

        return mustExec;
    }

    //==============================================================================
    /**
     * @brief Manipulate the input IR file and place it in the given buffer
     *
     * Applies filtering (EQ), time stretching (sample rate conversion) and gain to
     * internal IR channel buffers to prepare it for main audio processing, then write
     * channels to given output buffer.
     *
     * @param [out] irChannelOut    Processed impulse response channel
     *
     * @throws std::runtime_error
     */
    void IRPipeline::exec(juce::AudioSampleBuffer& irChannelOut)
    {
        // Execute pipeline on IR channel
        for (auto& filter : filters)
        {
            filter->exec(irChannel);
        }

        timeStretch->exec(irChannel);
        gain->exec(irChannel);

        try
        {
            preDelay->exec(irChannel);
        }
        catch (const std::exception& e)
        {
            std::string errMsg = "Skipping pre-delay step due to exception: ";
                        errMsg += e.what();
                
            logger.dualPrint(Logger::Level::Error, errMsg);
        }

        // Use move semantics to write to output IR channel
        irChannelOut = std::move(irChannel);

        // Disable mustExec flag
        mustExec = false;
    }

    //==============================================================================
    /**
     * @brief Loads an impulse response from disk or IR bank
     *
     * If given string is the name of an IR in IR bank, load that buffer. Otherwise,
     * look for IR on disk.
     *
     * @param [in] irName   Name or path of IR file
     *
     * @throws std::invalid_argument
     */
    void IRPipeline::loadIR(const std::string& irNameOrFilePath)
    {
        if (irNameOrFilePath.empty())
        {
            throw std::invalid_argument("Received invalid IR name/path");
        }

        if (irBank.buffers.find(irNameOrFilePath) != irBank.buffers.end())
        {
            loadIRFromBank(irNameOrFilePath);
        }
        else
        {
            loadIRFromDisk(irNameOrFilePath);
        }
    }

    //==============================================================================
    /**
     * @brief Loads an impulse response from provided IR bank
     *
     * Loads the appropriate channel from the selected impulse response (IR) in memory.
     * Since this operation involves copying an entire buffer and thus may be fairly heavy,
     * processing is suspended until completion.
     *
     * @param [in] irName   Name of banked IR file
     *
     * @throws std::invalid_argument
     */
    void IRPipeline::loadIRFromBank(const std::string& irName)
    {
        // Suspend processing since this may take a while
        processor->suspendProcessing(true);

        // Find requested IR
        const auto irIter = irBank.buffers.find(irName);
        const auto sampleRateIter = irBank.sampleRates.find(irName);

        if (irIter == irBank.buffers.end() ||
            sampleRateIter == irBank.sampleRates.end())
        {
            throw std::invalid_argument("Requested impulse response (" + irName + ") does not exist in IR bank");
        }

        const juce::AudioSampleBuffer& ir = irIter->second;
        double irSampleRate = sampleRateIter->second;

        // Read samples (limit to max. MAX_IR_LENGTH_S) into separate buffers for each channel
        int numSamples = std::min(ir.getNumSamples(),
                                  (int)std::ceil(irSampleRate * MAX_IR_LENGTH_S));

        irChannel.clear();

        // Read channel corresponding to pipeline channel index
        if (channelIdx < ir.getNumChannels())
        {
            irChannel.setSize(1, numSamples);
            irChannel.copyFrom(0, 0, ir.getReadPointer(channelIdx), numSamples);
        }

        // Set parameters based on current impulse response
        timeStretch->origIRSampleRate = irSampleRate;

        // Resume processing
        processor->suspendProcessing(false);
    }

    //==============================================================================
    /**
     * @brief Loads an impulse response from a file (.WAV or .AIFF) to internal representation
     *
     * Loads the selected impulse response (IR) from disk and splits it into individual buffers
     * for each channel. This operation is potentially very heavy so processing is suspended
     * until completion.
     *
     * @param [in] irFilePath   Path to impulse response file
     *
     * @throws std::invalid_argument
     */
    void IRPipeline::loadIRFromDisk(const std::string& irFilePath)
    {
        // Suspend processing since this may take a while
        processor->suspendProcessing(true);

        // Load impulse response file
        juce::File irFile(irFilePath);

        juce::AudioFormatManager formatMgr;
        formatMgr.registerBasicFormats();

        std::unique_ptr<juce::AudioFormatReader> reader(formatMgr.createReaderFor(irFile));

        if (!reader)
        {
            throw std::invalid_argument("Failed to create reader for IR file: " + irFilePath);
        }

        // Read samples (limit to max. MAX_IR_LENGTH_S) into separate buffers for each channel
        int numSamples = (int)std::min(reader->lengthInSamples,
                                       (juce::int64)std::ceil(reader->sampleRate * MAX_IR_LENGTH_S));

        irChannel.clear();

        // Read channel corresponding to pipeline channel index
        // TODO: (maybe if we finish earlier than expected) Currently only handles mono/stereo,
        //       should we duplicate left/right channels when # channels is higher?
        irChannel.setSize(1, numSamples);
        reader->read(&irChannel, 0, numSamples, 0, channelIdx == 0, channelIdx == 1);

        // Set parameters based on current impulse response
        timeStretch->origIRSampleRate = reader->sampleRate;

        // Resume processing
        processor->suspendProcessing(false);
    }
}
