/*
  ==============================================================================

    IRPipeline.cpp

  ==============================================================================
*/

#include "IRPipeline.h"

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
    }

}
