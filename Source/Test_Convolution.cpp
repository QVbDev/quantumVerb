/*
==============================================================================

Test_Convolution.cpp

==============================================================================
*/

#include "catch.hpp"

#include "PluginProcessor.h"
#include "Convolution.h"
#include "PluginProcessor.h"

#include <chrono>

/**
* How to write tests with Catch:
* https://github.com/catchorg/Catch2/blob/2bbba4f5444b7a90fcba92562426c14b11e87b76/docs/tutorial.md#writing-tests
*/

TEST_CASE("Use a Convolution object to convolve two buffers", "[Convolution]") {
    constexpr int SAMPLE_RATE = 88200;
    constexpr int NUM_CHANNELS = 1;
    constexpr std::chrono::milliseconds BLOCK_DURATION_MS(20);
    const int NUM_SAMPLES_PER_BLOCK = (int)std::ceil((BLOCK_DURATION_MS.count() / 1000.0) * SAMPLE_RATE);

    // Create Convolution object
    reverb::AudioProcessor processor;
    processor.setPlayConfigDetails(NUM_CHANNELS, NUM_CHANNELS,
                                   SAMPLE_RATE, NUM_SAMPLES_PER_BLOCK);

    REQUIRE(processor.getSampleRate() == SAMPLE_RATE);

    reverb::Convolution convolution(&processor);

    SECTION("Convolve known audio buffer with known impulse response") {
        constexpr std::chrono::milliseconds IR_DURATION_MS(1000);
        const int IR_NUM_SAMPLES = (int)std::ceil((IR_DURATION_MS.count() / 1000.0) * SAMPLE_RATE);

        // Create 1 audio block
        juce::AudioSampleBuffer audio(1, NUM_SAMPLES_PER_BLOCK);

        REQUIRE(audio.getNumChannels() == 1);
        REQUIRE(audio.getNumSamples() == NUM_SAMPLES_PER_BLOCK);

        for (int i = 0; i < NUM_SAMPLES_PER_BLOCK; ++i)
        {
            // Audio: step function starting at NUM_SAMPLES_PER_BLOCK / 2
            if (i < NUM_SAMPLES_PER_BLOCK / 2)
            {
                audio.setSample(0, i, 0);
            }
            else
            {
                audio.setSample(0, i, 1);
            }
        }

        // Create impulse response
        juce::AudioSampleBuffer ir(1, IR_NUM_SAMPLES);

        REQUIRE(ir.getNumChannels() == 1);
        REQUIRE(ir.getNumSamples() == IR_NUM_SAMPLES);

        for (int i = 0; i < IR_NUM_SAMPLES; ++i)
        {
            // Audio: step function starting at (IR_NUM_SAMPLES / 2)
            if (i < IR_NUM_SAMPLES / 2)
            {
                ir.setSample(0, i, 0);
            }
            else
            {
                ir.setSample(0, i, 1);
            }
        }

        // Run convolution
        convolution.loadIR(ir);
        convolution.exec(audio);

        // IR should be unchanged
        CHECK(ir.getNumChannels() == 1);
        CHECK(ir.getNumSamples() == IR_NUM_SAMPLES);

        for (int i = 0; i < IR_NUM_SAMPLES; ++i)
        {
            if (i < IR_NUM_SAMPLES / 2)
            {
                CHECK(ir.getSample(0, i) == 0);
            }
            else
            {
                CHECK(ir.getSample(0, i) == 1);
            }
        }

        // Compute expected output signal
        const int AUDIO_STEP_START = NUM_SAMPLES_PER_BLOCK / 2;
        const int AUDIO_STEP_END = NUM_SAMPLES_PER_BLOCK;

        const int IR_STEP_START = IR_NUM_SAMPLES / 2;
        const int IR_STEP_END = IR_NUM_SAMPLES;

        const int RESULT_RAMP_UP_START = AUDIO_STEP_START + IR_STEP_START - 1;
        const int RESULT_RAMP_UP_END = RESULT_RAMP_UP_START +
                                       std::min( AUDIO_STEP_END - AUDIO_STEP_START,
                                                 IR_STEP_END - IR_STEP_START );

        const int RESULT_PLATEAU_VAL = RESULT_RAMP_UP_END - RESULT_RAMP_UP_START;

        const int RESULT_RAMP_DOWN_START = RESULT_RAMP_UP_START +
                                           std::max( AUDIO_STEP_END - AUDIO_STEP_START,
                                                     IR_STEP_END - IR_STEP_START );

        // Validate output signal
        const int64_t NUM_SAMPLES_EXPECTED = NUM_SAMPLES_PER_BLOCK;

        CHECK(audio.getNumChannels() == 1);
        CHECK(audio.getNumSamples() == NUM_SAMPLES_EXPECTED);

        for (int i = 0; i < audio.getNumSamples(); ++i)
        {
            if (i < RESULT_RAMP_UP_START)
            {
                CHECK(audio.getSample(0, i) == 0);
            }
            else if (i < RESULT_RAMP_UP_END)
            {
                const int EXPECTED_VAL = i - RESULT_RAMP_UP_START;
                CHECK(audio.getSample(0, i) == EXPECTED_VAL);
            }
            else if (i < RESULT_RAMP_DOWN_START)
            {
                CHECK(audio.getSample(0, i) == RESULT_PLATEAU_VAL);
            }
            else
            {
                const int EXPECTED_VAL = RESULT_PLATEAU_VAL -
                                         (i - RESULT_RAMP_DOWN_START);

                CHECK(audio.getSample(0, i) == EXPECTED_VAL);
            }
        }
    }
}
