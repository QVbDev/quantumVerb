/*
  ==============================================================================

    LookAndFeel.h

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

namespace reverb
{

    class LookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        LookAndFeel();

        virtual juce::Typeface::Ptr getTypefaceForFont(const juce::Font&) override;

        virtual juce::Font getTextButtonFont(juce::TextButton& button, int buttonHeight) override;
        virtual juce::Font getLabelFont(juce::Label& label) override;

    };

}