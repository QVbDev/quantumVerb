/*
  ==============================================================================

    Test_Filter.cpp

  ==============================================================================
*/

#include "catch.hpp"

#include "Filter.h"
#include "../JuceLibraryCode/JuceHeader.h"

/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/2bbba4f5444b7a90fcba92562426c14b11e87b76/docs/tutorial.md#writing-tests
 */

//TEST_CASE("(TODO) Test case description", "[Filter]") {
//}

TEST_CASE("Filters are tested", "[filters]") {
	reverb::LowShelfFilter lowShelf();
	reverb::HighShelfFilter highShelf();
	reverb::PeakFilter peakFilter();

	




}
