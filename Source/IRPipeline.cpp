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
    IRPipeline::IRPipeline(juce::AudioProcessor * processor, int channelIdx)
        : Task(processor),
          channelIdx(channelIdx)
    {
        // Initialise pipeline steps
        equalizer = std::make_shared<Equalizer>(processor);
        timeStretch = std::make_shared<TimeStretch>(processor);
        gain = std::make_shared<Gain>(processor);
        preDelay = std::make_shared<PreDelay>(processor);
    }

    /**
     * @brief Updates parameters from processor parameter tree
     *
     * @param [in] params   Processor parameter tree
     * @param [in] blockId  ID of block whose paramters should be checked
     */
    void IRPipeline::updateParams(const juce::AudioProcessorValueTreeState& params,
                                  const juce::String&)
    {
        // Update pipeline parameters
        auto paramIRChoice = params.state.getChildWithName(AudioProcessor::PID_IR_FILE_CHOICE);
        if (paramIRChoice.getProperty("value") != irNameOrFilePath)
        {
            irNameOrFilePath = paramIRChoice.getProperty("value").toString().toStdString();
            mustExec = true;
        }

        // Update child parameters
        for (int i = 0; i < equalizer->getNumFilters(); i++)
        {
            std::string filterId = AudioProcessor::PID_FILTER_PREFIX + std::to_string(i);
            try 
            {
               equalizer->updateParams(params, filterId);
            }
            catch (const std::exception& e)
            {
                std::string errMsg = "Could not update filters due to exception: ";
                errMsg += e.what();

                logger.dualPrint(Logger::Level::Error, errMsg);
            }
        }

        gain->updateParams(params, AudioProcessor::PID_IR_GAIN);
        preDelay->updateParams(params, AudioProcessor::PID_PREDELAY);
        timeStretch->updateParams(params, AudioProcessor::PID_IR_LENGTH);
    }

    //==============================================================================
    /**
     * @brief Update sample rate for pipeline and child tasks
     * 
     * Compares new sample rate with previous value. If different, sets mustExec to
     * true in order to re-run pipeline for new sample rate. Store new sample rate
     * value in object.
     *
     * @param [in] sr   Sample rate
     */
    void IRPipeline::updateSampleRate(double sr)
    {
        if (sr != sampleRate)
        {
            sampleRate = sr;
            mustExec = true;

            for (auto& filter : filters)
            {
                filter->updateSampleRate(sr);
            }

            gain->updateSampleRate(sr);
            preDelay->updateSampleRate(sr);
            timeStretch->updateSampleRate(sr);
        }
    }

    //==============================================================================
    /**
     * @brief Check if this or any subtasks needs to be executed
     *
     * Pipeline must be executed if any IR-related parameters were changed since the
     * last run.
     *
     * @returns True if IRPipeline must be executed
     */
    bool IRPipeline::needsToRun() const
    {
        if (mustExec)
        {
            return true;
        }

        if (equalizer->needsToRun())
        {
            return true;
        }

        if (timeStretch->needsToRun())
        {
            return true;
        }

        if (gain->needsToRun())
        {
            return true;
        }

        if (preDelay->needsToRun())
        {
            return true;
        }

        return false;
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
    AudioBlock IRPipeline::exec(AudioBlock)
    {
        AudioBlock irBlock = reloadIR();

        // Apply filters
        equalizer->exec(irChannel);

        // Apply gain
        gain->exec(irBlock);

        // Resize buffer and apply timestretch
        timeStretch->prepareIR(ir);
        irBlock = ir;

        timeStretch->exec(irBlock);

        // Resize buffer and apply predelay
        preDelay->prepareIR(ir);
        irBlock = ir;

        preDelay->exec(irBlock);

        // Reset mustExec flag
        mustExec = false;

        // Return reference to processed IR channel
        return irBlock;
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
    AudioBlock IRPipeline::reloadIR()
    {
        if (irNameOrFilePath.empty())
        {
            throw std::invalid_argument("Received invalid IR name/path");
        }

        auto& irBank = IRBank::getInstance();
        if (irBank.buffers.find(irNameOrFilePath) != irBank.buffers.end())
        {
            loadIRFromBank(irNameOrFilePath);
        }
        else
        {
            loadIRFromDisk(irNameOrFilePath);
        }

        AudioBlock irBlock(ir);
        normalise(irBlock, MAX_IR_INTENSITY);

        return irBlock;
    }

    //==============================================================================
    /**
     * @brief Loads an impulse response from provided IR bank
     *
     * Loads the appropriate impulse response (IR) from binary data.
     *
     * @param [in] irName   Name of banked IR file
     *
     * @throws std::invalid_argument
     */
    void IRPipeline::loadIRFromBank(const std::string& irName)
    {
        auto& irBank = IRBank::getInstance();

        // Find requested IR
        const auto irIter = irBank.buffers.find(irName);
        const auto sampleRateIter = irBank.sampleRates.find(irName);

        if (irIter == irBank.buffers.end() ||
            sampleRateIter == irBank.sampleRates.end())
        {
            throw std::invalid_argument("Requested impulse response (" + irName + ") does not exist in IR bank");
        }

        // Copy IR buffer to internal representation
        ir.setSize(1, irIter->second.getNumSamples());

        ir.copyFrom(0, 0,
                    irIter->second.getReadPointer(channelIdx),
                    irIter->second.getNumSamples());
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
        // Load impulse response file
        juce::File irFile(irFilePath);

        juce::AudioFormatManager formatMgr;
        formatMgr.registerBasicFormats();

        std::unique_ptr<juce::AudioFormatReader> reader(formatMgr.createReaderFor(irFile));

        if (!reader)
        {
            throw std::invalid_argument("Failed to create reader for IR file: " + irFilePath);
        }

        // Read IR buffer into internal representation
        ir.setSize(reader->numChannels, (int)reader->lengthInSamples);
        reader->read(&ir, 0, (int)reader->lengthInSamples, 0, (channelIdx == 0), (channelIdx == 1));
    }

}
