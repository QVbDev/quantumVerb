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
        setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour(0xff9a9a9a));
        setColour(juce::Slider::ColourIds::trackColourId, juce::Colour(0xff0079c1));

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

    juce::Font LookAndFeel::getTextButtonFont(juce::TextButton&, int buttonHeight)
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

        font.setSizeAndStyle(std::min(16.0f, 0.9f * label.getHeight()),
                             font.getStyleFlags(),
                             font.getHorizontalScale(),
                             font.getExtraKerningFactor());

        return font;
    }

	void LookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
	{
        constexpr float pi = juce::MathConstants<float>::pi;

		// Geometric values
        juce::Rectangle<float> bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);

		float radius = std::min(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        float toAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        float lineW = std::min(8.0f, radius * 0.5f);
        float arcRadius = radius - lineW * 0.5f;

        float knobRadius = arcRadius - lineW * 0.1f;
        float knobPointerAngle = pi / 4.0f;
        
        float knobPointerHeight = radius - knobRadius + 1.0f;
        juce::Point<float> knobPointerTip(bounds.getCentreX(),
                                          bounds.getCentreY() - (knobPointerHeight + arcRadius));

        // Colours
        juce::Colour arcInactive = slider.findColour(juce::Slider::rotarySliderOutlineColourId);
        juce::Colour arcActive = slider.findColour(juce::Slider::trackColourId);
        juce::Colour fillOuter = slider.findColour(juce::Slider::rotarySliderFillColourId);
        juce::Colour fillInner = fillOuter.brighter();

        juce::ColourGradient fill(fillInner, bounds.getCentre(),
                                  fillOuter, bounds.getTopLeft(),
                                  true);

        auto arcStrokeType = juce::PathStrokeType(lineW,
                                                  juce::PathStrokeType::curved,
                                                  juce::PathStrokeType::EndCapStyle::butt);

        // Background arc
        juce::Path backgroundArc;
        backgroundArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(),
                                    arcRadius, arcRadius,
                                    0.0f,
                                    rotaryStartAngle, rotaryEndAngle,
                                    true);

        g.setColour(arcInactive);

        g.strokePath(backgroundArc, arcStrokeType);

        // Value arc
        if (slider.isEnabled())
        {
            juce::Path valueArc;
            valueArc.addCentredArc(bounds.getCentreX(),
                                   bounds.getCentreY(),
                                   arcRadius,
                                   arcRadius,
                                   0.0f,
                                   rotaryStartAngle,
                                   toAngle,
                                   true);

            g.setColour(arcActive);
            g.strokePath(valueArc, arcStrokeType);
        }

        // Knob
        juce::Path knob;
        knob.addCentredArc(bounds.getCentreX(), bounds.getCentreY(),
                           knobRadius, knobRadius,
                           0.0f,
                           knobPointerAngle / 2.0f,
                           2 * pi - knobPointerAngle / 2.0f,
                           true);

        knob.lineTo(knobPointerTip);
        knob.closeSubPath();

        knob.applyTransform(juce::AffineTransform::rotation(toAngle, bounds.getCentreX(), bounds.getCentreY()));

        g.setGradientFill(fill);
        g.fillPath(knob);

		return void();
	}

}
