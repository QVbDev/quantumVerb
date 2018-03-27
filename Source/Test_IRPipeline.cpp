/*
  ==============================================================================

    Test_IRPipeline.cpp

  ==============================================================================
*/

#include "catch.hpp"

#include "IRBank.h"
#include "IRPipeline.h"
#include "PluginProcessor.h"
#include "Logger.h"

#include <chrono>

/**
* How to write tests with Catch:
* https://github.com/catchorg/Catch2/blob/2bbba4f5444b7a90fcba92562426c14b11e87b76/docs/tutorial.md#writing-tests
*/

// TODO: Test parameter changes

//==============================================================================
/**
 * Mocked IRPipeline class for loading custom IR buffers in unit tests.
 */
class IRPipelineMocked : public reverb::IRPipeline
{
public:
    using IRPipeline::IRPipeline;

    //==============================================================================
    juce::String getIRName() { return irNameOrFilePath; }
};

TEST_CASE("Use an IRPipeline to manipulate an impulse response", "[IRPipeline]") {
    constexpr int IR_SAMPLE_RATE = 96000;
    constexpr int IR_NUM_CHANNELS = 2;
    constexpr std::chrono::milliseconds IR_DURATION_MS(5000);
    const int IR_NUM_SAMPLES = (int)std::ceil((IR_DURATION_MS.count() / 1000.0) * IR_SAMPLE_RATE);

    // Create IRPipeline object
    reverb::AudioProcessor processor;
    processor.setPlayConfigDetails(IR_NUM_CHANNELS, IR_NUM_CHANNELS,
                                   IR_SAMPLE_RATE, IR_NUM_SAMPLES);

    REQUIRE(processor.getSampleRate() == IR_SAMPLE_RATE);

    IRPipelineMocked irPipeline(&processor, 0);
    irPipeline.updateParams(processor.parameters);
    irPipeline.updateSampleRate(IR_SAMPLE_RATE);
    
    auto& irBank = reverb::IRBank::getInstance();
    REQUIRE(irPipeline.getIRName() == irBank.buffers.begin()->first);


    SECTION("IR processing shouldn't be excessively long") {
        constexpr std::chrono::milliseconds MAX_EXEC_TIME_MS(200);

        juce::AudioSampleBuffer ir(1, 512);

        // Measure processing time
        auto start = std::chrono::high_resolution_clock::now();
        irPipeline.exec(ir);
        auto end = std::chrono::high_resolution_clock::now();

        auto execTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        CHECK(execTime.count() < MAX_EXEC_TIME_MS.count());
    }
}
