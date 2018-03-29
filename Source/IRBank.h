/*
  ==============================================================================

    IRBank.h

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

#include <map>

namespace reverb
{

    class IRBank
    {
    public:
        //==============================================================================
        IRBank();

        IRBank(const IRBank&) = delete;
        IRBank& operator=(const IRBank&) = delete;

        //==============================================================================
        static const IRBank& getInstance();

        //==============================================================================
        const std::map<std::string, juce::AudioSampleBuffer>& buffers;
        const std::map<std::string, double>& sampleRates;

    protected:
        //==============================================================================
        void build();

        std::map<std::string, juce::AudioSampleBuffer> buffersModifiable;
        std::map<std::string, double> sampleRatesModifiable;
    };

}