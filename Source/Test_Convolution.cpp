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
    constexpr std::chrono::milliseconds MAX_EXEC_TIME(3000);

    // Prepare the Convolution object
    reverb::AudioProcessor processor;
    processor.setPlayConfigDetails(PROC_NUM_CHANNELS, PROC_NUM_CHANNELS,
        PROC_SAMPLE_RATE, PROC_NUM_SAMPLES_PER_BLOCK);

    REQUIRE(processor.getSampleRate() == PROC_SAMPLE_RATE);

    reverb::Convolution convolutionObject(&processor);

    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    SECTION("Convolve buffers of binary values") {
        std::string irFilePath = juce::File::getCurrentWorkingDirectory().getFullPathName().toStdString() +
            "/../../ImpulseResponses/ir.wav";
        std::string songFilePath = juce::File::getCurrentWorkingDirectory().getFullPathName().toStdString() +
            "/../../ImpulseResponses/audio.wav";

        juce::File irFile(irFilePath);
        REQUIRE(irFile.existsAsFile() == true);

        juce::File songFile(songFilePath);
        REQUIRE(songFile.existsAsFile() == true);

        std::shared_ptr<juce::AudioFormatReader> audioReader(formatManager.createReaderFor(songFile));
        // juce::AudioFormatReader* reader1 = formatManager.createReaderFor(songFile);
        REQUIRE(audioReader != nullptr);

        std::shared_ptr<juce::AudioFormatReader> irReader(formatManager.createReaderFor(irFile));
        // juce::AudioFormatReader* reader2 = formatManager.createReaderFor(irFile);
        REQUIRE(irReader != nullptr);

        juce::AudioSampleBuffer songBuffer(audioReader->numChannels, (int)audioReader->lengthInSamples);
        audioReader->read(&songBuffer, 0, (int)audioReader->lengthInSamples, 0, true, true);

        juce::AudioSampleBuffer irBuffer(irReader->numChannels, (int)irReader->lengthInSamples);
        irReader->read(&irBuffer, 0, (int)irReader->lengthInSamples, 0, true, true);

        REQUIRE((unsigned int)songBuffer.getNumChannels() == audioReader->numChannels);
        REQUIRE(songBuffer.getNumSamples() == audioReader->lengthInSamples);
        REQUIRE((unsigned int)irBuffer.getNumChannels() == irReader->numChannels);
        REQUIRE(irBuffer.getNumSamples() == irReader->lengthInSamples);

        auto start = std::chrono::high_resolution_clock::now();

        convolutionObject.loadIR(irBuffer);
        convolutionObject.exec(songBuffer);

        auto end = std::chrono::high_resolution_clock::now();
        auto execTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        REQUIRE(execTime.count() < MAX_EXEC_TIME.count());

        REQUIRE(songBuffer.getNumSamples() == audioReader->lengthInSamples);
        REQUIRE(songBuffer.getNumChannels() == 1);
    }
}
