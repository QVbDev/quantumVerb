/*
==============================================================================

Equalizer.cpp

==============================================================================
*/

#include "Equalizer.h"
#include "PluginProcessor.h"

namespace reverb {

    Equalizer::Equalizer(juce::AudioProcessor * processor, int numFilters)
        : Task(processor) 
    {

        if (numFilters < 3) numFilters = 3;
        if (numFilters > 6) numFilters = 6;

        filterSet.add(new LowShelfFilter(processor));
        
        for (int i = 0; i < numFilters - 2; i++) 
        {
            filterSet.add(new PeakFilter(processor));
        }

        filterSet.add(new HighShelfFilter(processor));

        for (int i = 0; i < numFilters; i++) 
        {
            EQGains.push_back(1.0f);
        }

        /*
        * The following is the default configuration of the Equalizer class: 
        * all filter frequencies are equally spaced between 0 and 10000 Hz 
        * with a gain of 2, and a Q of 0.71 for the shelf filters and 4 for the peak filters.
        */

        filterSet[0]->setQ(0.71);
        filterSet[numFilters - 1]->setQ(0.71);

        for (int i = 0; i < numFilters; i++)
        {
            EQGains[i] = 2;
            filterSet[i]->setGain(2);
            filterSet[i]->setFrequency((i + 1) * 10000 / numFilters);

            if (i != 0 && i != numFilters - 1) filterSet[i]->setQ(4);
        }

        

        
    }

    //==============================================================================
    /**
    * @brief Read processor parameters and update block parameters as necessary
    * @param [in] params   the value tree containing the parameters
    * @param [in] blockId  the Id of the relevant parameter block
    * @returns True if any parameters were changed, false otherwise.
    */

    void Equalizer::updateParams(const juce::AudioProcessorValueTreeState& params,
        const juce::String& blockId)
    {
        //Extract filter ID from block ID
        int filterId = std::stoi(blockId.getLastCharacters(1).toStdString());

        if (filterId < 0 || filterId >= filterSet.size()) throw InvalidFilterException();

        // Gain
        auto paramGain = params.getRawParameterValue(blockId + AudioProcessor::PID_FILTER_GAIN_SUFFIX);

        if (!paramGain)
        {
            throw std::invalid_argument("Parameter not found for gain factor in Filter block");
        }

        if (*paramGain != EQGains[filterId])
        {
            EQGains[filterId] = *paramGain;
            mustExec = true;
        }

        filterSet[filterId]->updateParams(params, blockId);

        mustExec |= filterSet[filterId]->needsToRun();

        if (mustExec)
        {
            calibrateFilters();
        }


    }

    /**
    * @brief Processes the AudioBuffer input with the EQ filters
    *
    * @param [in] ir   AudioBuffer to be processed
    */

    AudioBlock Equalizer::exec(AudioBlock ir) 
    {

        for (int i = 0; i < filterSet.size(); i++) 
        {
            filterSet[i]->exec(ir);
        }

        mustExec = false;

        return ir;
    }

    void Equalizer::updateSampleRate(double sr) {
        for (int i = 0; i < filterSet.size(); i++) {
            filterSet[i]->updateSampleRate(sr);
        }
    }

    /**
    * @brief Calibrates the individual filter gains so that the total gains are equal to the user defined values 
    *
    * This function solves a linear equation system of N variables where N = number of filters in order to find the individual gains that, when stacked together, 
    * are equal to the user specified values at the frequencies of 0, 21000 Hz and at each single one of the peak filter center frequencies. 
    *
    * The use of decibels is necessary for the gain stacks to behave linearly. The algorithm can be used iteratively and is detailed
    * in the Standford University lecture note available at https://ccrma.stanford.edu/courses/424/handouts.2004/424_Handout22_Filters4_LectureNotes.pdf
    *
    * @throws ConvergenceException
    */


    void Equalizer::calibrateFilters() 
    {


        //Update filter parameters before gain normalization

        const int dim = filterSet.size();

        std::vector<float> evalFrequencies(dim);

        //Set evaluation frequencies

        evalFrequencies[0] = 0;

        for (int i = 1; i < dim - 1; i++) 
        {
            evalFrequencies[i] = filterSet[i]->frequency;
        }

        evalFrequencies[(dim - 1)] = 21000;

        //Create Matrix objects

        juce::dsp::Matrix<float> B(dim, dim);
        juce::dsp::Matrix<float> gamma(dim, 1);
        juce::dsp::Matrix<float> lambda(dim, 1);

        float * B_data = B.getRawDataPointer();
        float * gamma_data = gamma.getRawDataPointer();
        float * lambda_data = lambda.getRawDataPointer();

        //Compute gamma column and set gains to 1 dB for B matrix

        for (int i = 0; i < dim; i++) 
        {
            gamma_data[i] = Filter::todB(EQGains[i]);
            filterSet[i]->setGain(Filter::invdB(1.0f));
        }


        bool unitaryGain = false;
        
        //Correction algorithm (5 iterations)

        for (int k = 0; k < 5; k++)
        {

            memcpy(lambda_data, gamma_data, dim * sizeof(float));

            //Compute B matrix with new gains
            for (int i = 0; i < dim; i++)
            {

                for (int j = 0; j < dim; j++)
                {

                    B_data[j + B.getNumColumns() * i] = filterSet[j]->getdBAmplitude(evalFrequencies[i]);
                }
            }

            B.solve(lambda);

            for (int i = 0; i < dim; i++)
            {

                if (std::abs(lambda_data[i]) < 0.001f)
                {
                    unitaryGain = true;
                }

                if (!(Filter::invdB(lambda_data[i] * Filter::todB(filterSet[i]->gainFactor)) > 0))
                {
                    throw ConvergenceException();
                }

                
                    filterSet[i]->setGain(Filter::invdB(lambda_data[i] * Filter::todB(filterSet[i]->gainFactor)));
            }

            if (unitaryGain)
            {
                break;
            }
        }


    }

    /**
    * @brief Returns the Equalizer amplitude response in dB at a given frequency
    *
    * @param [in] freq   Frequency at which the filter magnitude is evaluated
    */

    float Equalizer::getdBAmplitude(float freq) 
    {
        float dBAmplitude = 0;

        for (int i = 0; i < filterSet.size(); i++) 
        {
            dBAmplitude += filterSet[i]->getdBAmplitude(freq);
        }

        return dBAmplitude;

    }

    /**
    * @brief Returns the number of filters in the equalizer
    *
    */

    int Equalizer::getNumFilters() 
    {
        return filterSet.size();
    }

    bool Equalizer::needsToRun() const {

        return mustExec;
    }
}