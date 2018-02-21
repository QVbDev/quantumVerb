/*
  ==============================================================================

    PluginParameters.cpp
    Created: 20 Feb 2018 3:33:38pm
    Author:  Eric Seguin

  ==============================================================================
*/

#include "PluginParameters.h"

namespace reverb
{
 
    static const float gain15dB = juce::Decibels::decibelsToGain(15);

    static std::map<const std::string, juce::AudioProcessorParameterWithID *> params;

    //==============================================================================
    /**
     * @brief Returns a map of all plugin parameters
     *
     * Builds all plugin parameters with appropriate range and default values.
     * Parameters are dynamically allocated since JUCE's juce::AudioProcessor class
     * handles their deletion once they're added using AudioProcessor::addParameter().
     *
     * Since all default parameter values are set here, this is where changes to these
     * values should be made.
     *
     * @returns Map of all plugin parameters
     */
    const std::map<const std::string, juce::AudioProcessorParameterWithID*> getMapOfParams()
    {
        // Only build parameters once. This saves work and avoids memory leaks due to
        // allocating memory for the same parameter more than once.
        if (params.size() > 0)
        {
            return params;
        }

        /**
         * Filters
         */
        params["filter00_freq"] = new juce::AudioParameterFloat( "filter00_freq",
                                                                 "EQ: Low-shelf cut-off frequency",
                                                                 juce::NormalisableRange<float>(0, 2e4),
                                                                 1e3 );

        // TODO: Upper limit on Q factor?
        params["filter00_Q"]    = new juce::AudioParameterFloat( "filter00_Q",
                                                                 "EQ: Low-shelf Q factor",
                                                                 juce::NormalisableRange<float>(0.7, 1e4),
                                                                 0.707 );
        
        params["filter00_gain"] = new juce::AudioParameterFloat( "filter00_gain",
                                                                 "EQ: Low-shelf gain",
                                                                 juce::NormalisableRange<float>(0, gain15dB),
                                                                 1 );


        // TODO: Vary filter parameters by type (Alex?)
        params["filter01_freq"] = new juce::AudioParameterFloat( "filter01_freq",
                                                                 "EQ: Peak filter 1 cut-off frequency",
                                                                 juce::NormalisableRange<float>(0, 2e4),
                                                                 1e3 );

        // TODO: Upper limit on Q factor?
        params["filter01_Q"]    = new juce::AudioParameterFloat( "filter01_Q",
                                                                 "EQ: Peak filter 1 Q factor",
                                                                 juce::NormalisableRange<float>(0.7, 1e4),
                                                                 0.707 );
        
        params["filter01_gain"] = new juce::AudioParameterFloat( "filter01_gain",
                                                                 "EQ: Peak filter 1 gain",
                                                                 juce::NormalisableRange<float>(0, gain15dB),
                                                                 1 );


        // TODO: Vary filter parameters by type (Alex?)
        params["filter02_freq"] = new juce::AudioParameterFloat( "filter02_freq",
                                                                 "EQ: Peak filter 2 cut-off frequency",
                                                                 juce::NormalisableRange<float>(0, 2e4),
                                                                 1e3 );

        // TODO: Upper limit on Q factor?
        params["filter02_Q"]    = new juce::AudioParameterFloat( "filter02_Q",
                                                                 "EQ: Peak filter 2 Q factor",
                                                                 juce::NormalisableRange<float>(0.7, 1e4),
                                                                 0.707 );
        
        params["filter02_gain"] = new juce::AudioParameterFloat( "filter02_gain",
                                                                 "EQ: Peak filter 2 gain",
                                                                 juce::NormalisableRange<float>(0, gain15dB),
                                                                 1 );


        // TODO: Vary filter parameters by type (Alex?)
        params["filter03_freq"] = new juce::AudioParameterFloat( "filter03_freq",
                                                                 "EQ: High-shelf cut-off frequency",
                                                                 juce::NormalisableRange<float>(0, 2e4),
                                                                 1e3 );

        // TODO: Upper limit on Q factor?
        params["filter03_Q"]    = new juce::AudioParameterFloat( "filter03_Q",
                                                                 "EQ: High-shelf Q factor",
                                                                 juce::NormalisableRange<float>(0.7, 1e4),
                                                                 0.707 );
        
        params["filter03_gain"] = new juce::AudioParameterFloat( "filter03_gain",
                                                                 "EQ: High-shelf gain",
                                                                 juce::NormalisableRange<float>(0, gain15dB),
                                                                 1 );
        

        /**
         * IR gain
         */
        // TODO: Upper limit on IR gain?
        params["ir_gain"] = new juce::AudioParameterFloat( "ir_gain",
                                                           "Impulse response gain",
                                                           juce::NormalisableRange<float>(0, gain15dB),
                                                           1 );


        /**
         * Pre-delay
         */
        params["predelay"] = new juce::AudioParameterFloat( "predelay",
                                                            "Pre-delay (ms)",
                                                            juce::NormalisableRange<float>(0, 1000),
                                                            0 );


        /**
         * Dry/wet mixer
         */
        // TODO: Default wet ratio?
        params["wet_ratio"] = new juce::AudioParameterFloat( "wet_ratio",
                                                             "Dry/wet ratio (0 = all dry, 1 = all wet)",
                                                             juce::NormalisableRange<float>(0, 1),
                                                             0.5 );


        /**
         * Output gain
         */
        // TODO: Is range appropriate (i.e. do we want to allow boosting volume too)?
        params["out_gain"] = new juce::AudioParameterFloat( "out_gain",
                                                            "Output volume",
                                                            juce::NormalisableRange<float>(0, gain15dB),
                                                            1 );

        
        /**
         * Impulse responses
         */
        static const juce::StringArray irChoices = { "large_church.wav",
                                                     "large_hall.wav",
                                                     "medium_chamber.wav",
                                                     "medium_hall.wav",
                                                     "open_air.wav",
                                                     USER_CHOICE };

        params["ir_choices"] = new juce::AudioParameterChoice( "ir_choices",
                                                               "Impulse response choices",
                                                               irChoices,
                                                               0 );


        /**
         * END: Parent is responsible for deleting parameters on destruction
         */
        return params;
    };


    //==============================================================================
    /**
     * @brief Clears the global parameter map
     *
     * Empties the shared parameter map. This is useful to call in the destructors of
     * any objects that may delete our parameters. In particular, AudioProcessor takes
     * ownership of these parameters when AudioProcessor::addParameter is used.
     * 
     * Normally we probably don't have to worry about this but our unit tests sometimes
     * build multiple AudioProcessor objects, so we need this method to clean the slate
     * every time one is destroyed.
     */
    void resetMapOfParams()
    {
        // Empty map
        params.clear();

        // Rebuild map using new allocations for each parameter
        getMapOfParams();
    }

}