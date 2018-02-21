/*
  ==============================================================================

    IRPipeline.cpp

  ==============================================================================
*/

#include "IRPipeline.h"
#include "PluginParameters.h"
#include "Util.h"

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
     *
     * @throws std::invalid_argument
     */
    IRPipeline::IRPipeline(juce::AudioProcessor * processor, int channelIdx)
        : Task(processor),
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

        // Update parameters
        updateParams();

        // Look for IR bank
        // TODO: Choose impulse responses to provide in bank and select default one (current IR is temporary)
        irBank = juce::File::getSpecialLocation(juce::File::currentApplicationFile).getParentDirectory();

        std::string defaultIRFilePath = irBank.getFullPathName().toStdString() +
                                        "/ImpulseResponses/Resources/" + currentIR.toStdString();

        juce::File irFile(defaultIRFilePath);
        if (!irFile.existsAsFile())
        {
            throw std::invalid_argument("Failed to locate default impulse response: " + defaultIRFilePath);
        }

        loadIR(defaultIRFilePath);
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
    bool IRPipeline::updateParams(const std::string&)
    {
        auto& params = getMapOfParams();

        bool changedConfig = false;

        // Update pipeline parameters
        auto paramIRChoices = dynamic_cast<juce::AudioParameterChoice*>(params.at("ir_choices"));

        if (!paramIRChoices)
        {
            throw std::invalid_argument("Received non-choice parameter for IR choice in IRPipeline block");
        }

        if (*paramIRChoices != currentIR)
        {
            currentIR = *paramIRChoices;
            changedConfig = true;
        }

        // Update child parameters
        for (int i = 0; i < filters.size(); ++i)
        {
            std::string filterId = "filter0" + std::to_string(i);
            changedConfig |= filters[i]->updateParams(filterId);
        }

        changedConfig |= gain->updateParams("ir_gain");
        changedConfig |= preDelay->updateParams("predelay");

        // Update mustExec flag
        mustExec |= changedConfig;

        return changedConfig;
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

            // TODO: Let user know about error through UI (?)
        }

        // Use move semantics to write to output IR channel
        irChannelOut = std::move(irChannel);

        // Disable mustExec flag
        mustExec = false;
    }

    //==============================================================================
    /**
     * @brief Loads an impulse response from a file (.WAV or .AIFF) to internal representation
     *
     * Loads the selected impulse response (IR) from disk and splits it into individual buffers
     * for each channel.
     *
     * @param [in] irFilePath   Path to impulse response file
     *
     * @throws std::invalid_argument
     */
    void IRPipeline::loadIR(const std::string& irFilePath)
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
    }
}
