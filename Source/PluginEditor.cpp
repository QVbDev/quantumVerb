/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"

#include "PluginProcessor.h"

#include <string.h>

namespace reverb
{

    //==============================================================================
    /**
    * @brief Constructs an AudioProcessorEditor object associated with an AudioProcessor
    *
    * Creates an AudioProcessorEditor and each of its components. Constructs the UI by adding
    * all the required elements. It handles element placement, plugin window default size and
    * how to handle the resizing of the window.
    *
    * @param [in] processor    Pointer to main processor
    *
    */
    AudioProcessorEditor::AudioProcessorEditor(AudioProcessor& p)
        : juce::AudioProcessorEditor(&p), processor(p), parameters(p.parameters),
          headerBlock(p), graphBlock(p), reverbBlock(p),
          lowShelfFilterBlock(p, 0, "low-shelf filter"), peakLowFilterBlock(p, 1, "low-peak filter"),
          peakHighFilterBlock(p, 2, "high-peak filter"), highShelfFilterBlock(p, 3, "high-shelf filter"),
          filterBlocks({ &lowShelfFilterBlock, &peakLowFilterBlock, &peakHighFilterBlock, &highShelfFilterBlock })
    {
        // Make sure that before the constructor has finished, you've set the
        // editor's size to whatever you need it to be.
        setResizable(true, true);
        setResizeLimits(800, 640, 1920, 1080);

        // Look and feel
        setLookAndFeel(&lookAndFeel);

        // Display header block
        addAndMakeVisible(headerBlock);

        // Display graph block
        addAndMakeVisible(graphBlock);

        // Display reverb params block
        addAndMakeVisible(reverbBlock);

        // Build filter blocks
        filterBlockNames = { "low-shelf", "peak (low)", "peak (high)", "high-shelf" };

        for (auto filterBlock : filterBlocks)
        {
            addAndMakeVisible(*filterBlock);
        }

        // Set listeners for IR graph
        headerBlock.irChoice.addListener(&graphBlock);

        reverbBlock.irLength.addListener(&graphBlock);
        reverbBlock.preDelay.addListener(&graphBlock);
        reverbBlock.irGain.addListener(&graphBlock);
        reverbBlock.outGain.addListener(&graphBlock);
        reverbBlock.wetRatio.addListener(&graphBlock);

        for (auto& filterBlock : filterBlocks)
        {
            filterBlock->freq.addListener(&graphBlock);
            filterBlock->q.addListener(&graphBlock);
            filterBlock->gain.addListener(&graphBlock);
        }

        // Calls resized when creating UI to position all the elements as if window was resized.
        this->resized();
	}

    //==============================================================================
	AudioProcessorEditor::~AudioProcessorEditor()
	{
        setLookAndFeel(nullptr);
	}

	void AudioProcessorEditor::paint(juce::Graphics& g)
	{
		// (Our component is opaque, so we must completely fill the background with a solid colour)
		g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

		g.setColour(juce::Colours::white);
		g.setFont(15.0f);
	}

    //==============================================================================
    /**
    * @brief Manages the layout of AudioProcessorEditor when the window is resized
    *
    * This function defines all the relative positioning of the various UI elements.
    */
	void AudioProcessorEditor::resized()
	{
        juce::Rectangle<int> bounds(getLocalBounds());

        int width = bounds.getWidth();
        int height = bounds.getHeight();

        int padding = (int)(PADDING_REL * height);

        // Header block
        auto headerBounds = bounds;
        
        headerBounds.setBottom((int)(0.13 * height));

        headerBounds.reduce(padding, padding);

        headerBlock.setBounds(headerBounds);

        // Graph
        auto graphBounds = bounds;

        graphBounds.setTop(headerBounds.getBottom());
        graphBounds.setBottom(headerBounds.getBottom() + padding + (int)(0.52 * height));
        graphBounds.setRight((int)(0.75 * width));

        graphBounds.reduce(padding, padding);

        graphBlock.setBounds(graphBounds);

        // Reverb block
        auto reverbBounds = bounds;

        reverbBounds.setTop(headerBounds.getBottom());
        reverbBounds.setBottom(headerBounds.getBottom() + padding + (int)std::round(0.52 * height));
        reverbBounds.setLeft(graphBounds.getRight() + padding);

        reverbBounds.reduce(padding, padding);

        reverbBlock.setBounds(reverbBounds);

        // Filters
        auto filterBounds = bounds;

        filterBounds.setTop(reverbBounds.getBottom());
        filterBounds.setBottom(reverbBounds.getBottom() + padding + (int)std::round(0.35 * height));

        int filterBlockWidth = width / 4;

        for (int i = 0; i < 4; ++i)
        {
            auto thisFilterBounds = filterBounds.removeFromLeft(filterBlockWidth);
            thisFilterBounds.reduce(padding, padding);

            filterBlocks[i]->setBounds(thisFilterBounds);
        }
	}

}