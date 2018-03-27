/*
==============================================================================

Convolution.cpp

==============================================================================
*/

#include "Convolution.h"

namespace reverb
{

    //==============================================================================
    /**
    * @brief Constructor. Creates a Convolution object.
    *
    * @details Creates a Convolution object associated to an AudioProcessor.
    *
    * @param [in] processor    Pointer to main processor.
    */
    Convolution::Convolution(juce::AudioProcessor* processor)
        : Task(processor)
    {
    }

    //==============================================================================
    /**
     * @brief No parameters to update, do nothing
     */
    void Convolution::updateParams(const juce::AudioProcessorValueTreeState&,
                                   const juce::String&)
    {
    }

    //==============================================================================
    /**
     * @brief Main function of the Convolution class. Executes the convolution of
     *        the audio buffer with the IR.
     *
     * @details Creates a ProcessContext object that specifies the audio buffer to
     *          convolve and the buffer to contain the resulted signal. Calls the
     *          juce::dsp::Convolution::process() function to compute the
     *          convolution.
     *
     * @param [in,out] audio    The audio buffer to be convolved with the IR.
     */
    AudioBlock Convolution::exec(AudioBlock audio)
    {
        juce::dsp::ProcessContextReplacing<float> context(audio);
        process(context);

        return audio;
    }

    //==============================================================================
    /**
    * @brief Defines the main characteristics of the convolution process and loads
    *        the IR into the juce::dsp::Convolution object.
    *
    * @details Calls JUCE Convolution class's functions prepare() and
    *          copyAndLoadImpulseResponseFromBuffer().
    *
    * @param [in] ir    The IR signal to convolve with the audio buffer.
    */
    void Convolution::loadIR(AudioBlock ir)
    {
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = processor->getSampleRate();

        // If the block size is small, the overlap-add method requires a small amount of RAM
        // at the expense of increasing computational load. If the block size is large, the
        // overlap-add method is more efficient computationally, at the expense of the memory
        // required. It is relatively difficult to find an optimal block size, since the
        // implementation is machine-dependent.
        // https://books.google.ca/books?id=VQs_Ly4DYDMC&pg=PA130&lpg=PA130&dq=convolution+algorithm+optimal+block+size&source=bl&ots=jImfjKud-t&sig=SjLhgdAnfac0_6He7RBl0O-Snu8&hl=en&sa=X&ved=0ahUKEwj4y9367IDZAhWL24MKHV28AXoQ6AEIOzAD#v=onepage&q=convolution%20algorithm%20optimal%20block%20size&f=false
        spec.maximumBlockSize = 2048;
        spec.numChannels = (juce::uint32)ir.getNumChannels();

        // Must be called before loading the impulse response to provide to the convolution
        // the maximumBufferSize to handle and the sample rate for optional resampling.
        prepare(spec);

        copyAndLoadImpulseResponseFromBlock(ir, spec.sampleRate,
                                            ir.getNumChannels() == 2,
                                            false, false, 0);
    }
}