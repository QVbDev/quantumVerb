/*
==============================================================================

Equalizer.h

==============================================================================
*/

#pragma once

#include "Task.h"
#include "Filter.h"

#include <memory>
#include <exception>

enum{LOW, PEAK1, PEAK2, HIGH};

namespace reverb {

	class Equalizer : public Task 
	{

	public:

		Equalizer(juce::AudioProcessor * processor);

		virtual void exec(juce::AudioSampleBuffer& ir) override;

		void updateFilters();

		float getdBAmplitude(float freq);

		void setFilterFrequency(float freq, int num);
		void setFilterGain(float freq, int num);
		void setFilterQ(float freq, int num);


	private:


		juce::OwnedArray<Filter> filterSet;
	};

	struct InvalidFilterException : public std::exception {
		const char * what() const throw () {
			return "Equalizer: Filter ID is invalid";
		}
	};
}