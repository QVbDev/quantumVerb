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
        lowShelfFilterBlock(p, 0, "Low-Shelf Filter"), peakLowFilterBlock(p, 1, "Low-Peak Filter"),
        peakHighFilterBlock(p, 2, "High-Peak Filter"), highShelfFilterBlock(p, 3, "High-Shelf Filter")
	{
		// Make sure that before the constructor has finished, you've set the
		// editor's size to whatever you need it to be.
        setResizable(true, true);
        setResizeLimits(800, 640, 1920, 1080);

        // Look and feel
        setLookAndFeel(&lookAndFeel);

        // Display header block
        headerBlock.addListener(this);
        addAndMakeVisible(headerBlock);

        // Display right-side block
        addAndMakeVisible(reverbBlock);

        // Build filter blocks
        static constexpr const char * filterNames[] = { "low-shelf", "peak (low)", "peak (high)", "high-shelf" };

        addAndMakeVisible(lowShelfFilterBlock);
        addAndMakeVisible(peakLowFilterBlock);
        addAndMakeVisible(peakHighFilterBlock);
        addAndMakeVisible(highShelfFilterBlock);

        juce::Image eqGraph = juce::ImageFileFormat::loadFrom(BinaryData::graph_placeholder_png,
                                                              BinaryData::graph_placeholder_pngSize);

        juce::Image blank;

        graphButton.setImages(false, true, false,
                              eqGraph, 1.0, juce::Colours::transparentWhite, 
                              blank, 1.0, juce::Colours::transparentWhite,
                              blank, 1.0, juce::Colours::transparentWhite,
                              0);

        addAndMakeVisible(graphButton);

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

        graphButton.setBounds(graphBounds);

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

    // https://forum.juce.com/t/custom-graphics-sliders-and-knobs/4588/4

    void AudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
    {
        juce::ComboBox* irComboBox = headerBlock.getComboBox(comboBoxThatHasChanged);
        if (irComboBox->getComponentID() == processor.PID_IR_FILE_CHOICE) {
            // If the user chose to select a custom impulse response
            if (irComboBox->getSelectedItemIndex() == 0) {
                juce::AudioFormatManager formatManager;
                formatManager.registerBasicFormats();
                // Accepts the following file types: .wav, .bwf, .aiff, .flac, .ogg, .mp3, .wmv, .asf, .wm, .wma
                juce::FileChooser fileChooser("Select an impulse response",
                    juce::File::getCurrentWorkingDirectory(),
                    formatManager.getWildcardForAllFormats(),
                    false);
                if (fileChooser.browseForFileToOpen()) {
                    juce::File selectedFile = fileChooser.getResult();
                    processor.parameters.state.getChildWithName(processor.PID_IR_FILE_CHOICE)
                        .setProperty("value", selectedFile.getFullPathName(), nullptr);
                    irComboBox->setText(selectedFile.getFullPathName());
                }
                else {
                    irComboBox->setSelectedItemIndex(headerBlock.previousSelectedIR);
                }
            }
            // If the user chose to select an impulse response from the IR bank
            else {
                processor.parameters.state.getChildWithName(processor.PID_IR_FILE_CHOICE)
                    .setProperty("value", irComboBox->getItemText(irComboBox->getSelectedItemIndex()), nullptr);
            }
            headerBlock.previousSelectedIR = irComboBox->getSelectedItemIndex();
        }
    }

    // handler for button clicks
    void AudioProcessorEditor::buttonClicked(juce::Button* clickedButton) 
    {

    }

    // handler for slider interactions
    // TODO: add predelay handling
    void AudioProcessorEditor::sliderValueChanged(juce::Slider* changedSlider)
    {
        // All sliders are handled by parameter tree attachments
    }

}