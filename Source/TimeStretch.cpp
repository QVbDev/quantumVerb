/*
  ==============================================================================

    TimeStretch.cpp
    Created: 20 Jan 2018 5:00:14pm
    Author:  Eric Seguin

  ==============================================================================
*/

#include "TimeStretch.h"

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
        this->soundtouchHandle = soundtouch_createInstance();
        
        if (!this->soundtouchHandle)
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
        soundtouch_destroyInstance(this->soundtouchHandle);
    }

    //==============================================================================
    /**
     * @brief (TODO) Brief description
     *
     * (TODO) Detailed description
     *
     * @param [in,out] ir   (TODO) Parameter description
     */
    void TimeStretch::exec(juce::AudioSampleBuffer& ir)
    {
    }

}
