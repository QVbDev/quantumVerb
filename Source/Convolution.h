/*
==============================================================================

Convolution.h

==============================================================================
*/

#pragma once

#include "Task.h"

namespace reverb
{

	//==============================================================================
	/**
	 * Derives from JUCE's Convolution base class. Computes the convolution
	 * algorithm between the audio signal and the IR buffer.
	 */
	class Convolution : public Task,
		                protected juce::dsp::Convolution
	{
	public:
		//==============================================================================
		Convolution(juce::AudioProcessor* processor);

		//==============================================================================
		using Ptr = std::shared_ptr<Convolution>;

        //==============================================================================
        virtual void updateParams(const juce::AudioProcessorValueTreeState& params,
                                  const juce::String& = "") override;

        virtual AudioBlock exec(AudioBlock audio) override;

		//==============================================================================
		void loadIR(AudioBlock ir);
	};

}