/*
  ==============================================================================

    IRPipeline.cpp

  ==============================================================================
*/

#include "IRPipeline.h"

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
    IRPipeline::IRPipeline(juce::AudioProcessor * processor)
        : Task(processor)
    {
        // Initialise pipeline steps
        filters[0] = std::make_shared<LowShelfFilter>(processor);
        filters[1] = std::make_shared<PeakFilter>(processor);
        filters[2] = std::make_shared<PeakFilter>(processor);
        filters[3] = std::make_shared<HighShelfFilter>(processor);

        timeStretch = std::make_shared<TimeStretch>(processor);
        gain = std::make_shared<Gain>(processor);
        preDelay = std::make_shared<PreDelay>(processor);

        // Look for IR bank
        // TODO: Choose impulse responses to provide in bank and select default one (current IR is temporary)
        irFilePath = juce::File::getCurrentWorkingDirectory().getFullPathName().toStdString() +
                     "/../../ImpulseResponses/chiesa_di_san_lorenzo.wav";

        juce::File irFile(irFilePath);
        if (!irFile.existsAsFile())
        {
            throw std::invalid_argument("Failed to locate default impulse response: " + irFilePath);
        }
    }

    //==============================================================================
    /**
     * @brief Manipulate the input IR file and place it in the given buffer
     *
     * Loads the selected impulse response (IR) from disk and places it in the given
     * output buffer. Applies filtering (EQ), time stretching (sample rate conversion)
     * and gain to buffer to prepare it for main audio processing.
     *
     * @param [out] ir  Processed impulse response
     */
    void IRPipeline::exec(juce::AudioSampleBuffer& ir)
    {
        // Load impulse response file
        juce::File irFile(irFilePath);
        juce::FileInputStream irFileStream(irFile);

        if (irFileStream.failedToOpen())
        {
            throw std::invalid_argument("Failed to open IR file: " + irFilePath);
        }

        juce::WavAudioFormat wavFormat;
        juce::AudioFormatReader * reader = wavFormat.createReaderFor(&irFileStream, true);

        if (!reader)
        {
            throw std::invalid_argument("Failed to create reader for IR file: " + irFilePath);
        }

        // Read samples (limit to max. 5s) into separate buffers for each channel
        juce::int64 numSamples = std::min(reader->lengthInSamples,
                                         (juce::int64)std::ceil(reader->sampleRate * 5));

        std::vector<juce::AudioSampleBuffer> irChannels(2, juce::AudioSampleBuffer(1, numSamples));

        reader->read(&irChannels[0], 0, numSamples, 0, true, false);
        reader->read(&irChannels[1], 0, numSamples, 0, false, true);

        // Set parameters based on current impulse response
        timeStretch->origIRSampleRate = reader->sampleRate;
        
        // Execute pipeline on both channels
        for (auto& irChannel : irChannels)
        {
            for (auto& filter : filters)
            {
                filter->exec(irChannel);
            }

            timeStretch->exec(irChannel);
            gain->exec(irChannel);
            preDelay->exec(irChannel);
        }

        if (irChannels[0].getNumSamples() != irChannels[1].getNumSamples())
        {
            throw std::runtime_error( "Obtained non-identical number of samples in both channels (L = " +
                                      std::to_string(irChannels[0].getNumSamples()) +
                                      ", R = " +
                                      std::to_string(irChannels[1].getNumSamples()) +
                                      ")" );
        }

        // Merge channels into single IR buffer
        ir.clear();
        ir.setSize(irChannels.size(), irChannels[0].getNumSamples(), false, true, false);

        for (int i = 0; i < irChannels.size(); ++i)
        {
            std::copy(irChannels[i].getReadPointer(0),
                      irChannels[i].getReadPointer(0) + irChannels[i].getNumSamples(),
                      ir.getWritePointer(i));
        }
    }

}
