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
    void Convolution::exec(juce::AudioSampleBuffer& audio)
    {
        juce::dsp::AudioBlock<float> audioBlock(audio);
        juce::dsp::AudioBlock<float> outBlock(audio);
        juce::dsp::ProcessContextNonReplacing<float> context(audioBlock, outBlock);
        process(context);
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
    void Convolution::loadIR(juce::AudioSampleBuffer& ir)
    {
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = processor->getSampleRate(); // 44.1 kHz
        // If the block size is small, the overlap-add method requires a small amount of RAM
        // at the expense of increasing computational load. If the block size is large, the
        // overlap-add method is more efficient computationally, at the expense of the memory
        // required. It is relatively difficult to find an optimal block size , since the
        // implementation is machine-dependent.
        // https://books.google.ca/books?id=VQs_Ly4DYDMC&pg=PA130&lpg=PA130&dq=convolution+algorithm+optimal+block+size&source=bl&ots=jImfjKud-t&sig=SjLhgdAnfac0_6He7RBl0O-Snu8&hl=en&sa=X&ved=0ahUKEwj4y9367IDZAhWL24MKHV28AXoQ6AEIOzAD#v=onepage&q=convolution%20algorithm%20optimal%20block%20size&f=false
        spec.maximumBlockSize = 1024;
        spec.numChannels = ir.getNumChannels();
        // Must be called before loading the impulse response to provide to the convolution
        // the maximumBufferSize to handle and the sample rate for optional resampling.
        prepare(spec);
        copyAndLoadImpulseResponseFromBuffer(ir, spec.sampleRate,
            (ir.getNumChannels() == 1 ? false : true), false, false, 0);
    }
}