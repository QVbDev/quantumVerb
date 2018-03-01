/*
  ==============================================================================
  
    PluginEditor.cpp

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "GUI_filter_box.h"
#include "GUI_reverb_box.h"
#include "LookAndFeel.h"
#include "PluginProcessor.h"

namespace reverb
{

	//==============================================================================
	/**
    * Implements the plugins UI. This includes window configuration, placement of UI elements and handlers for 
    * various events such as slider interaction.
	*/
	class AudioProcessorEditor : public juce::AudioProcessorEditor
                               , public juce::Button::Listener 
                               , public juce::Slider::Listener
                               , public juce::TextEditor::Listener
	{
	public:
		AudioProcessorEditor(AudioProcessor&);
		~AudioProcessorEditor();

		//==============================================================================
		void paint(juce::Graphics&) override;
		void resized() override;

        void buttonClicked(juce::Button * button) override;
        /*void loadIR(int num);
        void handleMenuResult(int result);
        void menuCallback(int result);*/
        void sliderValueChanged(juce::Slider *changedSlider) override;

        struct Cell
        {
            //==============================================================================
            Cell(float w = 1.0f) : widthPercent(w) {}

            //==============================================================================
            double widthPercent;

            //==============================================================================
            double offsetX = 0;
        };

        struct Row
        {
            //==============================================================================
            Row(float h = 1.0f, std::vector<Cell> c = {}) : heightPercent(h), cells(c) {}

            //==============================================================================
            double heightPercent;
            std::vector<Cell> cells;

            //==============================================================================
            double offsetY = 0;
        };

        struct Layout
        {
            //==============================================================================
            double padding;
            std::vector<Row> rows;
        };

        static const Layout& getLayout();

	protected:
        //==============================================================================
		AudioProcessor& processor;
        juce::AudioProcessorValueTreeState& parameters;

        //==============================================================================
        LookAndFeel lookAndFeel;

        //==============================================================================
        using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
        using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

        //==============================================================================
        juce::TextButton isOn;
        juce::Label isOnLabel;
        std::unique_ptr<ButtonAttachment> isOnAttachment;

        juce::TextButton irChoice;
        juce::Label irChoiceLabel;

        juce::TextEditor sampleRate;
        juce::Label sampleRateLabel;

        //==============================================================================
        GUI_reverb_box reverbParam;

        //==============================================================================
        GUI_filter_box lowShelf;
        GUI_filter_box peakingLow;
        GUI_filter_box peakingHigh;
        GUI_filter_box highShelf;

    private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessorEditor)
	};

}