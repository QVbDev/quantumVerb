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
     * @brief (TODO) Brief description
     *
     * (TODO) Detailed description
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

        // Look for IR bank
        irFilePath = juce::File::getCurrentWorkingDirectory().getFullPathName().toStdString() +
                     "/../../ImpulseResponses/chiesa_di_san_lorenzo.wav";

        juce::File irBank(irFilePath);
        if (!irBank.existsAsFile())
        {
            throw std::invalid_argument("Failed to locate IR bank");
        }

        // TODO: Initialise IR file path to some default value
    }

    //==============================================================================
    /**
     * @brief (TODO) Brief description
     *
     * (TODO) Detailed description
     *
     * @param [in,out] ir   (TODO) Parameter description
     */
    void IRPipeline::exec(juce::AudioSampleBuffer& ir)
    {
        // Load IR file
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

        // Create AudioBuffer with max 5s of samples
        juce::int64 numSamples = std::min(reader->lengthInSamples,
                                         (juce::int64)std::ceil(reader->sampleRate * 5));

        bool useLeftChannel = true;
        bool useRightChannel = true;

        ir.clear();
        ir.setSize((int)useLeftChannel + (int)useRightChannel, numSamples, false, true, false);

        reader->read(&ir, 0, numSamples, 0, useLeftChannel, useRightChannel);
        
        // Execute pipeline
        for (auto& filter : filters)
        {
            filter->exec(ir);
        }

        // FIXME: time stretch seems to cause an infinite loop in AudioProcessor unit tests,
        //        investigate this ASAP
        //timeStretch->exec(ir);
        gain->exec(ir);
    }

}
