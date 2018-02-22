/*
  ==============================================================================

    Test_Main.cpp

  ==============================================================================
*/

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "JuceHeader.h"

// Main test file, should be left empty
int main(int argc, char * argv[])
{
    // Initialise JUCE framework
    juce::ScopedJuceInitialiser_GUI juceLib;

    // Run Catch tests
    int result = Catch::Session().run(argc, argv);

    return result;
}