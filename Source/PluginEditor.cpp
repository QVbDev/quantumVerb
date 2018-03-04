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
          headerBlock(p), reverbBlock(p),
          lowShelfFilterBlock(p, 0), peakLowFilterBlock(p, 1),
          peakHighFilterBlock(p, 2), highShelfFilterBlock(p, 3)
	{
		// Make sure that before the constructor has finished, you've set the
		// editor's size to whatever you need it to be.
        setResizable(true, true);
        setResizeLimits(800, 640, 1920, 1080);

        // Look and feel
        setLookAndFeel(&lookAndFeel);

        // Display header block
        addAndMakeVisible(headerBlock);

        // Display right-side block
        addAndMakeVisible(reverbBlock);

        // Build filter blocks
        static constexpr const char * filterNames[] = { "low-shelf", "peak (low)", "peak (high)", "high-shelf" };

        addAndMakeVisible(lowShelfFilterBlock);
        addAndMakeVisible(peakLowFilterBlock);
        addAndMakeVisible(peakHighFilterBlock);
        addAndMakeVisible(highShelfFilterBlock);

        // Calls resized when creating UI to position all the elements as if window was resized.
        this->resized();
	}

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

        //headerBox.drawAt(g, 0, 0, 100);
		//g.drawRoundedRectangle(0, 15, 300, 175, 2, 1);

	}    

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
        graphBounds.setBottom(headerBounds.getBottom() + padding + (int)(0.485 * height));
        graphBounds.setRight((int)(0.75 * width));

        graphBounds.reduce(padding, padding);

        // TODO: Place graph component

        // Reverb block
        auto reverbBounds = bounds;

        reverbBounds.setTop(headerBounds.getBottom());
        reverbBounds.setBottom(headerBounds.getBottom() + padding + 0.485 * height);
        reverbBounds.setLeft(graphBounds.getRight() + padding);

        reverbBounds.reduce(padding, padding);

        reverbBlock.setBounds(reverbBounds);

        // Filters
        UIFilterBlock * filters[4] = { &lowShelfFilterBlock, &peakLowFilterBlock,
                                       &peakHighFilterBlock, &highShelfFilterBlock };

        auto filterBounds = bounds;

        filterBounds.setTop(reverbBounds.getBottom());
        filterBounds.setBottom(reverbBounds.getBottom() + padding + 0.385 * height);

        int filterBlockWidth = width / 4;

        for (int i = 0; i < 4; ++i)
        {
            auto thisFilterBounds = filterBounds.removeFromLeft(filterBlockWidth);
            thisFilterBounds.reduce(padding, padding);

            filters[i]->setBounds(thisFilterBounds);
        }
	}

    // handler for button clicks
    void AudioProcessorEditor::buttonClicked(juce::Button*) 
    {
        // Most buttons are handled by parameter tree attachments

        // TODO: Handler for IR selection?
    }

    // Heavily inspired by JUCE standaloneFilterWindow.h askUserToLoadState()
    /** Pops up a dialog letting the user re-load the processor's state from a file. */

    // TODO: complete drop downmenu file explorer file selection
    /*void AudioProcessorEditor::loadIR(int num)
    {
        FileChooser fc("Load IR file");

        if (fc.browseForFileToOpen())
        {

            processor.irPipeline->irFilePath = fc.getResult().getFullPathName().toStdString();
        }
    }

    void AudioProcessorEditor::handleMenuResult(int result)
    {
        switch (result)
        {
        case 1:  this->loadIR(0) ; break;
        //case 2:  pluginHolder->askUserToSaveState(); break;
        //case 3:  pluginHolder->askUserToLoadState(); break;
        //case 4:  resetToDefaultState(); break;
        default: break;
        }
    }

    void AudioProcessorEditor::menuCallback(int result)
    {
        if (button != nullptr && result != 0)
            button->handleMenuResult(result);
    }*/

}