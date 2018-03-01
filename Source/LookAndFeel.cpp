/*
  ==============================================================================

    LookAndFeel.cpp

  ==============================================================================
*/

#include "LookAndFeel.h"

#include "Logger.h"

#include "BinaryData.h"

namespace reverb
{

    LookAndFeel::LookAndFeel()
    {
        setColour(juce::ResizableWindow::ColourIds::backgroundColourId, juce::Colour(0xff212121));

        setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colour(0x00000000));
        setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colour(0xff141414));
        setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour(0xff0079c1));
        setColour(juce::Slider::ColourIds::trackColourId, juce::Colour(0xff0079c1));
        setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour(0xff03a9f4));

        setColour(juce::ToggleButton::ColourIds::tickColourId, juce::Colour(0xff212121));
        setColour(juce::ToggleButton::ColourIds::tickDisabledColourId, juce::Colour(0xffcccccc));

        setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour(0x00000000));
        setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colour(0xffffffff));
        setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colour(0xff03a9f4));
        setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colour(0xffffffff));

        setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colour(0x00000000));
        setColour(juce::TextEditor::ColourIds::focusedOutlineColourId, juce::Colour(0xff808080));
        setColour(juce::TextEditor::ColourIds::highlightColourId, juce::Colour(0xff0079c1));
        setColour(juce::TextEditor::ColourIds::highlightedTextColourId, juce::Colour(0xffffffff));
        setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colour(0xff808080));
        setColour(juce::TextEditor::ColourIds::shadowColourId, juce::Colour(0x00000000));
        setColour(juce::TextEditor::ColourIds::textColourId, juce::Colour(0xffffffff));
    }

    juce::Typeface::Ptr LookAndFeel::getTypefaceForFont(const juce::Font& font)
    {
        static auto ubuntuL = juce::Typeface::createSystemTypefaceFor(BinaryData::UbuntuLight_ttf,
                                                                      BinaryData::UbuntuLight_ttfSize);
        
        static auto ubuntuB = juce::Typeface::createSystemTypefaceFor(BinaryData::UbuntuBold_ttf,
                                                                      BinaryData::UbuntuBold_ttfSize);

        if (font.isBold())
        {
            return ubuntuB;
        }
        else
        {
            return ubuntuL;
        }
    }

    juce::Font LookAndFeel::getTextButtonFont(juce::TextButton& button, int buttonHeight)
    {
        static juce::Font tmp(15.0f, juce::Font::bold);
        
        juce::Font font(getTypefaceForFont(tmp));

        font.setSizeAndStyle(std::min(15.0f, 0.6f * buttonHeight),
                             font.getStyleFlags(),
                             font.getHorizontalScale(),
                             font.getExtraKerningFactor());

        return font;
    }

    juce::Font LookAndFeel::getLabelFont(juce::Label& label)
    {
        static juce::Font tmp;
        
        juce::Font font(getTypefaceForFont(tmp));

        font.setSizeAndStyle(16.0f,
                                font.getStyleFlags(),
                                font.getHorizontalScale(),
                                font.getExtraKerningFactor());

        return font;
    }

}