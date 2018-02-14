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

        // Create AudioBuffer with max. 5s of samples
        juce::int64 numSamples = std::min(reader->lengthInSamples,
                                         (juce::int64)std::ceil(reader->sampleRate * 5));

        bool useLeftChannel = true;
        bool useRightChannel = true;

        ir.clear();
        ir.setSize((int)useLeftChannel + (int)useRightChannel, numSamples, false, true, false);

        reader->read(&ir, 0, numSamples, 0, useLeftChannel, useRightChannel);

        // Set parameters based on current impulse response
        timeStretch->origIRSampleRate = reader->sampleRate;
        
        // Execute pipeline
        for (auto& filter : filters)
        {
            filter->exec(ir);
        }

        timeStretch->exec(ir);
        gain->exec(ir);
        preDelay->exec(ir);
    }

}
