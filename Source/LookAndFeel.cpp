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

    juce::Font LookAndFeel::getLabelFont(juce::Label&)
    {
        static juce::Font tmp;
        
        juce::Font font(getTypefaceForFont(tmp));

        font.setSizeAndStyle(16.0f,
                                font.getStyleFlags(),
                                font.getHorizontalScale(),
                                font.getExtraKerningFactor());

        return font;
    }

	void LookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
	{
		
		//Geometric values
		auto radius = juce::jmin(width / 2, height / 2) - 9.0f;
		auto centreX = x + width * 0.5f;
		auto centreY = y + height * 0.5f;
		auto rx = centreX - radius;
		auto ry = centreY - radius;
		auto rw = radius * 2.0f;
		auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);


		//knob level indicator
		juce::Path p2;
		float arcThickness = 1.25f;
		juce::Rectangle<float> arcBounds = juce::Rectangle<float>::Rectangle(centreX - (radius * arcThickness), centreY - (radius * arcThickness), radius*arcThickness*2.0f, radius*arcThickness*2.0f);
		p2.addPieSegment(arcBounds, rotaryStartAngle, angle-0.1, 0.8f);
		g.setColour(juce::Colours::deepskyblue);
		g.fillPath(p2);

		// fill
		g.setColour(juce::Colours::grey);
		g.fillEllipse(rx, ry, rw, rw);

		// outline
		g.setColour(juce::Colours::blue);
		g.drawEllipse(rx, ry, rw, rw, 1.0f);

		// pointer
		juce::Path p;
		auto pointerLength = radius * 0.44f;
		auto pointerThickness = 3.5f;
		juce::Line<float> arrowLine = juce::Line<float>::Line(-pointerThickness + pointerThickness * 0.5f, -pointerLength, -pointerThickness - pointerThickness * 0.5f, -radius * 1.4f);
		p.addArrow(arrowLine, pointerThickness, pointerThickness*10.0f, radius*0.8f);
		p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
		g.setColour(juce::Colours::grey);
		g.fillPath(p);

		return void();
	}

}
