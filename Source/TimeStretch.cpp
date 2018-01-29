/*
  ==============================================================================

    TimeStretch.cpp
    Created: 20 Jan 2018 5:00:14pm
    Author:  Eric Seguin

  ==============================================================================
*/

#include "TimeStretch.h"

#include <algorithm>
#include <thread>

namespace reverb
{

    //==============================================================================
    /**
     * @brief Constructs a TimeStretch object associated with an AudioProcessor
     *
     * Creates a TimeStretch object with a handle to a SoundTouch instance
     *
     * @param [in] processor    Pointer to main processor
     *
     * @throws std::runtime_error
     */
    TimeStretch::TimeStretch(juce::AudioProcessor * processor)
        : Task(processor), origIRSampleRate(0)
    {
        soundtouchHandle = soundtouch_createInstance();
        
        if (!soundtouchHandle)
        {
            throw std::runtime_error("Failed to create SoundTouch handle");
        }
    }

    //==============================================================================
    /**
    * @brief Destroys a TimeStretch object with its internal SoundTouch handle
    */
    TimeStretch::~TimeStretch()
    {
        soundtouch_destroyInstance(soundtouchHandle);
    }

    //==============================================================================
    /**
     * @brief Apply time stretching algorithm to input IR buffer to change sample rate
     *
     * Stretch or compress input buffer by a factor proportional to original and desired
     * sample rates.
     *
     * @param [in,out] ir   Audio sample buffer to process
     */
    void TimeStretch::exec(juce::AudioSampleBuffer& ir)
    {
        soundtouch_setChannels(soundtouchHandle, 1);
        soundtouch_setSampleRate(soundtouchHandle, (unsigned)origIRSampleRate);

        // Move input data to new buffer; ir becomes output buffer for SoundTouch processing
        juce::AudioSampleBuffer irIn = std::move(ir);

        // Calculate tempo change & expected number of samples in output buffer
        float tempoChange = (float)(origIRSampleRate / processor->getSampleRate()) - 1.0f;
        unsigned newNumSamples = (unsigned)std::ceil(irIn.getNumSamples() / (1.0 + tempoChange));

        ir.setSize(irIn.getNumChannels(), newNumSamples, false, true);

        // TODO: Currently looping over channels, but SoundTouch offers multi-channel handling
        //       if buffers are arranged properly in memory. Is this efficient enough or should
        //       we look into matching SoundTouch's expected memory layout to use their channel
        //       handling?
        for (int channel = 0; channel < irIn.getNumChannels(); ++channel)
        {
            // Prepare SoundTouch processor
            soundtouch_clear(soundtouchHandle);
            soundtouch_setTempoChange(soundtouchHandle, tempoChange * 100.0f);

            // Put block in processor
            soundtouch_putSamples(soundtouchHandle,
                                  irIn.getReadPointer(channel), irIn.getNumSamples());

            // Wait for processing to complete
            unsigned curSample = 0;
            unsigned nbSamplesReceived = 0;

            do
            {
                curSample += nbSamplesReceived;

                // Write processed samples to output buffer
                auto curWritePtr = &ir.getWritePointer(channel)[curSample];
                
                nbSamplesReceived = soundtouch_receiveSamples(soundtouchHandle,
                                                              curWritePtr,
                                                              ir.getNumSamples() - curSample);
            }
            while (nbSamplesReceived != 0 &&
                   soundtouch_numUnprocessedSamples(soundtouchHandle) != 0);

            // Get last remaining samples from SoundTouch pipeline, if any
            if (soundtouch_numUnprocessedSamples(soundtouchHandle) != 0)
            {
                soundtouch_flush(soundtouchHandle);
                
                curSample += soundtouch_numSamples(soundtouchHandle);

                // Write processed samples to output buffer
                auto curWritePtr = &ir.getWritePointer(channel)[curSample];

                soundtouch_receiveSamples(soundtouchHandle,
                                          curWritePtr, ir.getNumSamples() - curSample);
            }
        }
    }

}
