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

	struct EQParameters {
		std::array<float, 4> frequencySet;
		std::array<float, 4> gainSet;
		std::array<float, 4> QSet;
	};

	class Equalizer : public Task 
	{

	public:

		Equalizer(juce::AudioProcessor * processor);

		virtual void exec(juce::AudioSampleBuffer& ir) override;

		void updateFilters();

		float getdBAmplitude(float freq);

		void setFilterFrequency(float freq, int num, bool update = true);
		void setFilterGain(float freq, int num, bool update = true);
		void setFilterQ(float freq, int num, bool update = true);


	private:


		juce::OwnedArray<Filter> filterSet;

		EQParameters parameters;
	};

	struct InvalidFilterException : public std::exception {
		const char * what() const throw () {
			return "Equalizer: Filter ID is invalid";
		}
	};
}