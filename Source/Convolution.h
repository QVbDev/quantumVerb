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
	* @brief Derives from JUCE's Convolution base class. Computes the convolution
	*        algorithm between the audio signal and the IR buffer.
	*/
	class Convolution : public Task,
		public juce::dsp::Convolution
	{
	public:
		//==============================================================================
		Convolution(juce::AudioProcessor* processor);

		//==============================================================================
		using Ptr = std::shared_ptr<Convolution>;

		//==============================================================================
		virtual void exec(juce::AudioSampleBuffer& audio) override;

		//==============================================================================
		void loadIR(juce::AudioSampleBuffer& ir);
	};

}