/*
  ==============================================================================

    Test_TimeStretch.cpp

  ==============================================================================
*/

#include "catch.hpp"

#include "PluginProcessor.h"
#include "TimeStretch.h"

/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/2bbba4f5444b7a90fcba92562426c14b11e87b76/docs/tutorial.md#writing-tests
 */

TEST_CASE("Use a TimeStretch object to manipulate a buffer", "[TimeStretch]") {
    reverb::AudioProcessor processor;
    reverb::TimeStretch timeStretch(&processor);

    timeStretch.origIRSampleRate = 44100;
    REQUIRE(timeStretch.origIRSampleRate == 44100);
}
