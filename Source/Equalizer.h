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

		void setFilterFrequency(float freq, int num);
		void setFilterGain(float freq, int num);
		void setFilterQ(float freq, int num);


	private:


		juce::OwnedArray<Filter> filterSet;

		std::array<float, 4> frequencySet;
		std::array<float, 4> gainSet;
		std::array<float, 4> QSet;
	};

	struct InvalidFilterException : public std::exception {
		const char * what() const throw () {
			return "Equalizer: Filter ID is invalid";
		}
	};

	template<typename T> class Matrix {
	public:

		Matrix(int h, int w): height(h), width(w) {
			if (h < 0 || w < 0)
				throw InvalidIndexException();

			array = new T[height * width];
		}

		~Matrix() {
			delete [] array;
		}

		T operator()(int x, int y) {
			if (x < 0 || y < 0 || x >= height || y >= width)
				throw InvalidIndexException();

			return array[y + width * x];
		}

		T get(int x, int y) {
			if (x < 0 || y < 0 || x >= height || y >= width)
				throw InvalidIndexException();

			return array[y + width * x];
		}

		void set(T elem, int x, int y) {
			if (x < 0 || y < 0 || x >= height || y >= width)
				throw InvalidIndexException();

			array[y + width * x] = elem; 
		}

	private:
		int height;
		int width;
		T * array;
	};

	struct InvalidIndexException : public std::exception {
		const char * what() const throw () {
			return "Matrix: Index is out of bounds";
		}
	};
}