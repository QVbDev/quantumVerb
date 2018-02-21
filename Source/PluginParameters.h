/*
  ==============================================================================

    PluginParameters.h

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

#include <map>
#include <string>

namespace reverb
{

    //==============================================================================
    static constexpr const char * USER_CHOICE = "";

    //==============================================================================
    const std::map<const std::string, juce::AudioProcessorParameterWithID*> getMapOfParams();
    void resetMapOfParams();

}