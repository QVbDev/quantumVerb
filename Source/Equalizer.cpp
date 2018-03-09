/*
==============================================================================

Equalizer.cpp

==============================================================================
*/

#include "Equalizer.h"

namespace reverb {

    Equalizer::Equalizer(juce::AudioProcessor * processor, int numFilters)
        : Task(processor) 
    {

        if (numFilters < 3) throw WrongFilterNumber();

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

        //These constructor setters will likely need to be updated in the very near future.

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

    /**
    * @brief Processes the AudioBuffer input with the EQ filters
    *
    * @param [in] ir   AudioBuffer to be processed
    */

    void Equalizer::exec(juce::AudioSampleBuffer& ir) 
    {
        calibrateFilters();

        for (int i = 0; i < filterSet.size(); i++) 
        {
            filterSet[i]->exec(ir);
        }
    }

    /**
    * @brief Calibrates the individual filter gains so that the stacked gains equal the user specified values at the frequencies of 0, 21000 Hz 
    *  and at every one of the peak filter center frequencies. 
    *
    * This function solves a linear equation system of N variables where N = number of filters.  The use of decibels is necessary for the gain stacks to behave linearly. 
    *
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

        evalFrequencies[(dim - 1)] = FMAX;

        //Create Matrix objects

        juce::dsp::Matrix<float> B(dim, dim);
        juce::dsp::Matrix<float> gamma(dim, 1);
        juce::dsp::Matrix<float> lambda(dim, 1);

        float * B_data = B.getRawDataPointer();
        float * gamma_data = gamma.getRawDataPointer();
        float * lambda_data = lambda.getRawDataPointer();

        //Compute gamma column

        for (int i = 0; i < dim; i++) 
        {
            gamma_data[i] = filterSet[i]->getdBAmplitude(evalFrequencies[i]);
        }

        //Set gains to 1 dB for B matrix
        for (int i = 0; i < dim; i++) 
        {
            filterSet[i]->setGain(Filter::invdB(1.0f));
        }
        


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

                filterSet[i]->setGain(Filter::invdB(lambda_data[i] * Filter::todB(filterSet[i]->gainFactor)));
            }
        }


    }

    /**
    * @brief Gives the Equalizer amplitude response in dB at a given frequency
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
    * @brief Sets the frequency of one of the equalizer's specified filter
    *
    * @param [in] freq  new frequency
    * @param [in] num   Filter ID
    */

    void Equalizer::setFilterFrequency(float freq, int num) 
    {
        if (num < 0 || num >= filterSet.size()) throw InvalidFilterException();

        //Check whether filter frequency crosses another one's

        if (num == 0) 
        {
            if (freq >= filterSet[num + 1]->frequency) throw WrongEQFrequency();
        }

        if (num == (filterSet.size() - 1))
        {
            if (freq <= filterSet[num - 1]->frequency) throw WrongEQFrequency();
        }

        else 
        {
            if (freq <= filterSet[num - 1]->frequency || freq >= filterSet[num + 1]->frequency) throw WrongEQFrequency();
        }

            filterSet[num]->setFrequency(freq);
    }

    /**
    * @brief Sets the gain of one of the equalizer's specified filter. The gain will be changed after calibration.
    *
    * @param [in] freq  new gain
    * @param [in] num   Filter ID
    */
    void Equalizer::setFilterGain(float gain, int num) 
    {
        if (num < 0 || num >= filterSet.size()) throw InvalidFilterException();

            EQGains[num] = gain;

            filterSet[num]->setGain(gain);

    }

    /**
    * @brief Sets the Q of one of the equalizer's specified filter
    *
    * @param [in] freq  new Q
    * @param [in] num   Filter ID
    */
    void Equalizer::setFilterQ(float Q, int num) 
    {
        if (num < 0 || num >= filterSet.size()) throw InvalidFilterException();

            filterSet[num]->setQ(Q);
    }

    /**
    * @brief Returns the cut-off/central frequency of one of the equalizer's specified filter
    *
    * @param num    Filter ID
    * @return       Cut-off/central frequency of the filter
    */

    float Equalizer::getFilterFrequency(int num) 
    {

        if (num < 0 || num >= filterSet.size()) throw InvalidFilterException();

        return filterSet[num]->frequency;
    }

    /**
    * @brief Returns the band gain of one of the equalizer's specified filter. Gain will change after calibration.
    *
    * @param num    Filter ID
    * @return       Gain of the filter
    */

    float Equalizer::getFilterGain(int num) 
    {
        if (num < 0 || num >= filterSet.size()) throw InvalidFilterException();

        return filterSet[num]->gainFactor;
    }

    /**
    * @brief Returns the Q of one of the equalizer's specified filter
    *
    * @param num    Filter ID
    * @return       Q of the filter
    */

    float Equalizer::getFilterQ(int num) 
    {
        if (num < 0 || num >= filterSet.size()) throw InvalidFilterException();

        return filterSet[num]->gainFactor;
    }

    /**
    * @brief Enables the specified filter
    */
    
    void Equalizer::enableFilter(int num) 
    {
        filterSet[num]->enable();
    }

    /**
    * @brief Disables the specified filter
    */
    void Equalizer::disableFilter(int num)
    {
        filterSet[num]->disable();
    }

    /**
    * @brief Returns the EQ gains as specified by the user (differs from actual filter gains after calibration
    *
    * @param [in] num   Filter ID
    * @return           EQ nominal gain
    */
    float Equalizer::getEQGain(int num) 
    {
        if (num < 0 || num >= filterSet.size()) throw InvalidFilterException();

        return EQGains[num];
    }

    /**
    * @brief Returns the number of filters in the EQ
    *
    */

    int Equalizer::getNumFilters() 
    {
        return filterSet.size();
    }
}