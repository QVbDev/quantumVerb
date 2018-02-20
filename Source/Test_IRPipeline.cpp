/*
  ==============================================================================

    Test_IRPipeline.cpp

  ==============================================================================
*/

#include "catch.hpp"

#include "IRPipeline.h"
#include "PluginProcessor.h"
#include "Util.h"

#include <chrono>

//==============================================================================
/**
 * Mocked IRPipeline class for loading custom IR buffers in unit tests.
 */
class IRPipelineMocked : public reverb::IRPipeline
{
public:
    using IRPipeline::IRPipeline;

    //==============================================================================
    /**
     * @brief Copies an impulse response buffer to internal representation
     *
     * Writes the given IR buffer to a temporary file on disk, then uses IRPipeline::loadIR
     * to read it into the object's internal representation.
     *
     * @param [in] ir   Impulse response sample buffer
     */
    void loadIRBuffer(juce::AudioSampleBuffer& ir, double sampleRate)
    {
        // Create temporary output file
        juce::File fileOut = juce::File::getCurrentWorkingDirectory().getChildFile("tmp_ir.wav");
        fileOut.create();

        {// Local scope to write temporary output file
            // NB: We don't need to handle freeing this stream since JUCE's writer will do it for us
            //     (unpleasant but that's how it is)
            juce::FileOutputStream * fileOutStream = fileOut.createOutputStream();

            if (fileOutStream->failedToOpen())
            {
                throw std::runtime_error("Failed to open temporary output file for mocked IRPipeline");
            }

            // Write IR to disk
            juce::WavAudioFormat wavFormat;
        
            std::unique_ptr<juce::AudioFormatWriter> writer(
                wavFormat.createWriterFor( fileOutStream, sampleRate, ir.getNumChannels(),
                                            wavFormat.getPossibleBitDepths()[0],
                                            (juce::StringPairArray()),
                                            0 )
            );

            if (!writer)
            {
                throw std::runtime_error("Failed to create writer for temporary output file in mocked IRPipeline");
            }

            writer->writeFromAudioSampleBuffer(ir, 0, ir.getNumSamples());
        }

        // Load temporary IR file
        loadIR(fileOut.getFullPathName().toStdString());

        // Clean up
        fileOut.deleteFile();
    }
};

/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/2bbba4f5444b7a90fcba92562426c14b11e87b76/docs/tutorial.md#writing-tests
 */

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

    IRPipelineMocked irPipeline(&processor);


    SECTION("IR processing shouldn't be excessively long") {
        constexpr std::chrono::seconds MAX_EXEC_TIME_MS(200);

        juce::AudioSampleBuffer ir;

        // Measure processing time
        auto start = std::chrono::high_resolution_clock::now();
        irPipeline.exec(ir);
        auto end = std::chrono::high_resolution_clock::now();

        auto execTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        REQUIRE(execTime.count() < MAX_EXEC_TIME_MS.count());
    }


    SECTION("IR should be limited to MAX_IR_LENGTH_MS") {
        constexpr int MAX_NUM_SAMPLES = IR_SAMPLE_RATE * irPipeline.MAX_IR_LENGTH_S;

        juce::AudioSampleBuffer irIn(1, MAX_NUM_SAMPLES + 1);

        REQUIRE(irIn.getNumChannels() == 1);
        REQUIRE(irIn.getNumSamples() > MAX_NUM_SAMPLES);

        irPipeline.loadIRBuffer(irIn, IR_SAMPLE_RATE);

        juce::AudioSampleBuffer irOut(1, 0);

        REQUIRE(irOut.getNumChannels() == 1);
        REQUIRE(irOut.getNumSamples() == 0);

        irPipeline.exec(irOut);

        REQUIRE(irOut.getNumSamples() == MAX_NUM_SAMPLES);
    }
}
