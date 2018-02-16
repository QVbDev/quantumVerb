/*
==============================================================================

Test_Convolution.cpp

==============================================================================
*/

#include "catch.hpp"

#include "PluginProcessor.h"
#include "Convolution.h"

#include <chrono>

/**
* How to write tests with Catch:
* https://github.com/catchorg/Catch2/blob/2bbba4f5444b7a90fcba92562426c14b11e87b76/docs/tutorial.md#writing-tests
*/

TEST_CASE("Verify the convolution process executed by the Convolution object", "[Convolution]") {
	// Test config
	constexpr auto PROC_SAMPLE_RATE = 44100; // Hz
	constexpr auto PROC_NUM_CHANNELS = 2;
	constexpr auto PROC_NUM_SAMPLES_PER_BLOCK = 1024;
	constexpr std::chrono::milliseconds MAX_EXEC_TIME_MS(10);

	// Prepare the Convolution object
	reverb::AudioProcessor processor;
	processor.setPlayConfigDetails(PROC_NUM_CHANNELS, PROC_NUM_CHANNELS,
		PROC_SAMPLE_RATE, PROC_NUM_SAMPLES_PER_BLOCK);

	REQUIRE(processor.getSampleRate() == PROC_SAMPLE_RATE);

	reverb::Convolution convolutionObject(&processor);

	juce::AudioFormatManager formatManager;
	formatManager.registerBasicFormats();

	SECTION("Convolve a real song with IR") {
		std::string irFilePath = juce::File::getCurrentWorkingDirectory().getFullPathName().toStdString() +
			"/../../ImpulseResponses/IRSacristyCathedral.wav";
		std::string songFilePath = juce::File::getCurrentWorkingDirectory().getFullPathName().toStdString() +
			"/../../ImpulseResponses/Blue(DaBaDee).wav";

		juce::File irFile(irFilePath);
		REQUIRE(irFile.existsAsFile() == true);

		juce::File songFile(songFilePath);
		REQUIRE(songFile.existsAsFile() == true);

		juce::AudioFormatReader* reader1 = formatManager.createReaderFor(songFile);
		REQUIRE(reader1 != nullptr);

		juce::AudioFormatReader* reader2 = formatManager.createReaderFor(irFile);
		REQUIRE(reader2 != nullptr);

		juce::AudioSampleBuffer songBuffer(reader1->numChannels, (int)reader1->lengthInSamples);
		reader1->read(&songBuffer, 0, (int)reader1->lengthInSamples, 0, true, true);

		juce::AudioSampleBuffer irBuffer(reader2->numChannels, (int)reader2->lengthInSamples);
		reader2->read(&irBuffer, 0, (int)reader2->lengthInSamples, 0, true, true);

		REQUIRE((unsigned int)songBuffer.getNumChannels() == reader1->numChannels);
		REQUIRE(songBuffer.getNumSamples() == reader1->lengthInSamples);
		REQUIRE((unsigned int)irBuffer.getNumChannels() == reader2->numChannels);
		REQUIRE(irBuffer.getNumSamples() == reader2->lengthInSamples);

#ifdef MEASURE_PERFORMANCE
		auto start = std::chrono::high_resolution_clock::now();
#endif

		convolutionObject.loadIR(irBuffer);
		convolutionObject.exec(songBuffer);

#ifdef MEASURE_PERFORMANCE
		auto end = std::chrono::high_resolution_clock::now();
		auto execTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		REQUIRE(execTime.count() < MAX_EXEC_TIME_MS.count());
#endif

		REQUIRE(songBuffer.getNumSamples() == (reader1->lengthInSamples + reader2->lengthInSamples + 1));
		REQUIRE(songBuffer.getNumChannels() == 2);

		juce::WavAudioFormat* wavFormat = new juce::WavAudioFormat();

		std::string outputFilePath = juce::File::getCurrentWorkingDirectory().getFullPathName().toStdString() +
			"/../../ImpulseResponses/result.wav";

		juce::File outputFile(outputFilePath);
		juce::FileOutputStream* outputTo = outputFile.createOutputStream();

		juce::AudioFormatWriter* writer = wavFormat->createWriterFor(outputTo, 44100, 1, 16, NULL, 0);
		writer->writeFromAudioSampleBuffer(songBuffer, 0, songBuffer.getNumSamples());
		delete outputTo;

		delete reader1;
		delete reader2;
	}

	SECTION("Convolve buffers of binary values") {
		std::string irFilePath = juce::File::getCurrentWorkingDirectory().getFullPathName().toStdString() +
			"/../../ImpulseResponses/ir.wav";
		std::string songFilePath = juce::File::getCurrentWorkingDirectory().getFullPathName().toStdString() +
			"/../../ImpulseResponses/audio.wav";

		juce::File irFile(irFilePath);
		REQUIRE(irFile.existsAsFile() == true);

		juce::File songFile(songFilePath);
		REQUIRE(songFile.existsAsFile() == true);

		juce::AudioFormatReader* reader1 = formatManager.createReaderFor(songFile);
		REQUIRE(reader1 != nullptr);

		juce::AudioFormatReader* reader2 = formatManager.createReaderFor(irFile);
		REQUIRE(reader2 != nullptr);

		juce::AudioSampleBuffer songBuffer(reader1->numChannels, (int)reader1->lengthInSamples);
		reader1->read(&songBuffer, 0, (int)reader1->lengthInSamples, 0, true, true);

		juce::AudioSampleBuffer irBuffer(reader2->numChannels, (int)reader2->lengthInSamples);
		reader2->read(&irBuffer, 0, (int)reader2->lengthInSamples, 0, true, true);

		REQUIRE((unsigned int)songBuffer.getNumChannels() == reader1->numChannels);
		REQUIRE(songBuffer.getNumSamples() == reader1->lengthInSamples);
		REQUIRE((unsigned int)irBuffer.getNumChannels() == reader2->numChannels);
		REQUIRE(irBuffer.getNumSamples() == reader2->lengthInSamples);

		auto start = std::chrono::high_resolution_clock::now();

		convolutionObject.loadIR(irBuffer);
		convolutionObject.exec(songBuffer);

		auto end = std::chrono::high_resolution_clock::now();
		auto execTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		REQUIRE(execTime.count() < MAX_EXEC_TIME_MS.count());

		REQUIRE(songBuffer.getNumSamples() == (reader1->lengthInSamples + reader2->lengthInSamples + 1));
		REQUIRE(songBuffer.getNumChannels() == 2);
	}
}
