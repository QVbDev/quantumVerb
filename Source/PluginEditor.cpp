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
        headerBlock.irChoice.addListener(this);

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

    /**
    * @brief   Callback function to open the popup menu asynchronously.
    *
    * @details This function will open a popup menu when the user clicks
    *          on the IR selection's button. The first item in the menu
    *          will let the user choose a custom IR from a file browser,
    *          while the other items will display the IRs from the IR bank.
    * @param   [in] clickedButton  Pointer to the button clicked by the user.
    */
    void AudioProcessorEditor::buttonClicked(juce::Button* clickedButton)
    {
        if (clickedButton != nullptr && clickedButton == &headerBlock.irChoice) {
            juce::PopupMenu m;
            m.addItem(1, "Load custom IR...");
            m.addSeparator();
            for (auto irFile : IRBank::getInstance().buffers) {
                m.addItem(m.getNumItems() + 1, irFile.first);
            }

            m.showMenuAsync(juce::PopupMenu::Options(),
                juce::ModalCallbackFunction::forComponent(menuCallback, &headerBlock));
        }
    }

    /**
    * @brief   Callback function to open the popup menu asynchronously. 
    *
    * @details This function will open a file browser if the user clicked on the first
    *          item in the dropdown menu. Any other item will load the appropriate IR
    *          from the IR bank.
    * @param   [in] result        The index of the item selected by the user.
    *          [in] headerBlock   Pointer to the header block.
    */
    void AudioProcessorEditor::menuCallback(int result, UIHeaderBlock* headerBlock)
    {
        if (headerBlock != nullptr && result != 0) {
            if (result == 1) {
                juce::AudioFormatManager formatManager;
                formatManager.registerBasicFormats();
                // Accepts the following file types: .wav, .bwf, .aiff, .flac, .ogg, .mp3, .wmv, .asf, .wm, .wma
                juce::FileChooser fileChooser("Select an impulse response",
                    juce::File::getCurrentWorkingDirectory(),
                    formatManager.getWildcardForAllFormats(),
                    false);
                if (fileChooser.browseForFileToOpen()) {
                    juce::File selectedFile = fileChooser.getResult();
                    headerBlock->parameters.state.getChildWithName(AudioProcessor::PID_IR_FILE_CHOICE)
                        .setProperty("value", selectedFile.getFullPathName(), nullptr);
                    headerBlock->irChoice.setButtonText(selectedFile.getFullPathName());
                }
            }
            else {
                juce::String irFile = std::next(IRBank::getInstance().buffers.begin(), result - 2)->first;
                headerBlock->parameters.state.getChildWithName(AudioProcessor::PID_IR_FILE_CHOICE)
                    .setProperty("value", irFile, nullptr);
                headerBlock->irChoice.setButtonText(irFile);
            }
        }
    }

}