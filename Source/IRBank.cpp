/*
  ==============================================================================

    IRBank.cpp

  ==============================================================================
*/

#include "IRBank.h"

namespace reverb
{

    //==============================================================================
    /**
     * @brief Construct IR bank
     *
     * Sets public buffer & sample rate references and calls build() method to populate
     * IR bank.
     */
    IRBank::IRBank()
        : buffers(buffersModifiable), sampleRates(sampleRatesModifiable)
    {
        build();
    }

    //==============================================================================
    /**
     * @brief Build IR bank from binary data
     *
     * Finds all audio resources and builds sample buffers for each. Rejects any
     * resources with the wrong format (only keep WAVE or AIFF).
     */
    void IRBank::build()
    {
        if (!buffersModifiable.empty() && !sampleRatesModifiable.empty())
        {
            return;
        }
        else if (buffersModifiable.size() != sampleRatesModifiable.size())
        {
            buffersModifiable.clear();
            sampleRatesModifiable.clear();
        }

        for (int i = 0; i < BinaryData::namedResourceListSize; ++i)
        {
            // Prepare memory input stream
            int dataSize;
            const char * data;

            data = BinaryData::getNamedResource(BinaryData::namedResourceList[i], dataSize);

            auto dataStream = new juce::MemoryInputStream(data, dataSize, false);

            // Attempt to create an audio format reader for stream
            juce::AudioFormatManager formatMgr;
            formatMgr.registerBasicFormats();

            std::unique_ptr<juce::AudioFormatReader> reader(formatMgr.createReaderFor(dataStream));

            // If reader was successfully created, this is a valid audio resource
            if (reader)
            {
                int numChannels = reader->numChannels;
                int numSamples = (int)reader->lengthInSamples;

                buffersModifiable[BinaryData::namedResourceList[i]] = juce::AudioSampleBuffer(numChannels, numSamples);
                reader->read(&buffersModifiable[BinaryData::namedResourceList[i]], 0, numSamples, 0, true, true);

                sampleRatesModifiable[BinaryData::namedResourceList[i]] = reader->sampleRate;
            }
        }
    }

}