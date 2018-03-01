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
    const AudioProcessorEditor::Layout& AudioProcessorEditor::getLayout()
    {
        static Layout layout;

        if (!layout.rows.empty())
        {
            return layout;
        }

        // Build layout
        layout.padding = 0.02;

        // Row 1: Header
        layout.rows.emplace_back(0.05f * (1 - 5 * layout.padding),
                                    std::vector<Cell>(3));

        layout.rows[0].cells[0].widthPercent = 0.10f * (1 - 3 * layout.padding);
        layout.rows[0].cells[1].widthPercent = 0.50f * (1 - 3 * layout.padding);
        layout.rows[0].cells[2].widthPercent = 0.40f * (1 - 3 * layout.padding);

        // Row 2: Graph & reverb parameters
        layout.rows.emplace_back(0.5f * (1 - 5 * layout.padding),
                                    std::vector<Cell>(2));

        layout.rows[1].cells[0].widthPercent = 0.60f * (1 - 2.5 * layout.padding);
        layout.rows[1].cells[1].widthPercent = 0.40f * (1 - 2.5 * layout.padding);

        // Row 3: Filter parameters
        layout.rows.emplace_back(0.45f * (1 - 5 * layout.padding),
                                    std::vector<Cell>(4));

        layout.rows[2].cells[0].widthPercent = 0.25f * (1 - 3.5 * layout.padding);
        layout.rows[2].cells[1].widthPercent = 0.25f * (1 - 3.5 * layout.padding);
        layout.rows[2].cells[2].widthPercent = 0.25f * (1 - 3.5 * layout.padding);
        layout.rows[2].cells[3].widthPercent = 0.25f * (1 - 3.5 * layout.padding);

        return layout;
    }

    AudioProcessorEditor::AudioProcessorEditor(AudioProcessor& p)
        : juce::AudioProcessorEditor(&p), processor(p), parameters(p.parameters),
          lowShelf(p, 0), peakingLow(p, 1), peakingHigh(p, 2), highShelf(p, 3),
          reverbParam(p)
	{
		// Make sure that before the constructor has finished, you've set the
		// editor's size to whatever you need it to be.
        setResizable(true, true);
        setResizeLimits(600, 600, 1920, 1080);

        // Look and feel
        setLookAndFeel(&lookAndFeel);

        // on/off button config
        isOn.setButtonText("STATE");
        isOn.setComponentID(p.PID_ACTIVE);
        isOn.setClickingTogglesState(true);

        addAndMakeVisible(isOn);

        isOnLabel.setText("reverb", juce::NotificationType::dontSendNotification);
        isOnLabel.setJustificationType(juce::Justification::topLeft);
        isOnLabel.attachToComponent(&isOn, false);

        isOnAttachment.reset(new ButtonAttachment(processor.parameters,
                                                  isOn.getComponentID(),
                                                  isOn));

        // IR file box config
        juce::String currentIR = p.parameters.state
                                             .getChildWithName(p.PID_IR_FILE_CHOICE)
                                             .getProperty("value");

        irChoice.setButtonText(currentIR);
        addAndMakeVisible(irChoice);

        irChoiceLabel.setText("impulse response", juce::NotificationType::dontSendNotification);
        irChoiceLabel.setJustificationType(juce::Justification::topLeft);
        irChoiceLabel.attachToComponent(&irChoice, false);

        // sample rate box config
        auto sampleRateTemp = std::to_string(p.getSampleRate() / 1000);
        sampleRate.setText(sampleRateTemp.substr(0, 4) + " kHz");
        sampleRate.setJustification(juce::Justification::centredLeft);
        sampleRate.setReadOnly(true);

        sampleRate.setFont(lookAndFeel.getTypefaceForFont(juce::Font()));

        addAndMakeVisible(sampleRate);

        sampleRateLabel.setText("sample rate", juce::NotificationType::dontSendNotification);
        sampleRateLabel.setJustificationType(juce::Justification::topLeft);
        sampleRateLabel.attachToComponent(&sampleRate, false);

        // display right-side sliders
        addAndMakeVisible(reverbParam);

        // display filter sliders
        addAndMakeVisible(lowShelf);
        addAndMakeVisible(peakingLow);
        addAndMakeVisible(peakingHigh);
        addAndMakeVisible(highShelf);

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

    // sets the layout of displayed components
	void AudioProcessorEditor::resized()
	{
        const Layout& layout = getLayout();

        // Get relative label height
        float labelHeight = isOnLabel.getFont().getHeight();
        float relLabelHeight = (labelHeight / getHeight());

        float offsetY = layout.padding;

        // Row 1: Header
        {
            offsetY += 2 * relLabelHeight;

            float offsetX = layout.padding;

            isOn.setBoundsRelative(offsetX, offsetY,
                                   layout.rows[0].cells[0].widthPercent,
                                   layout.rows[0].heightPercent);

            offsetX += layout.rows[0].cells[0].widthPercent;
            offsetX += layout.padding / 2;

            irChoice.setBoundsRelative(offsetX, offsetY,
                                       layout.rows[0].cells[1].widthPercent,
                                       layout.rows[0].heightPercent);

            offsetX += layout.rows[0].cells[1].widthPercent;
            offsetX += layout.padding / 2;

            sampleRate.setBoundsRelative(offsetX, offsetY,
                                         layout.rows[0].cells[2].widthPercent,
                                         layout.rows[0].heightPercent);
        }

        // Row 2: Graph & reverb parameters
        {
            offsetY += layout.rows[0].heightPercent;
            offsetY += layout.padding;

            float offsetX = layout.padding;

            // TODO: Add graph

            offsetX += layout.rows[1].cells[0].widthPercent;
            offsetX += layout.padding / 2;

            reverbParam.setBoundsRelative(offsetX, offsetY,
                                          layout.rows[1].cells[1].widthPercent,
                                          layout.rows[1].heightPercent);
        }

        // Row 3: EQ parameters
        {
            offsetY += layout.rows[1].heightPercent;
            offsetY += layout.padding;

            float offsetX = layout.padding;

            lowShelf.setBoundsRelative(offsetX, offsetY,
                                       layout.rows[2].cells[0].widthPercent,
                                       layout.rows[2].heightPercent);

            offsetX += layout.rows[2].cells[0].widthPercent;
            offsetX += layout.padding / 2;

            peakingLow.setBoundsRelative(offsetX, offsetY,
                                         layout.rows[2].cells[1].widthPercent, layout.rows[2].heightPercent);

            offsetX += layout.rows[2].cells[1].widthPercent;
            offsetX += layout.padding / 2;

            peakingHigh.setBoundsRelative(offsetX, offsetY,
                                          layout.rows[2].cells[2].widthPercent, layout.rows[2].heightPercent);

            offsetX += layout.rows[2].cells[2].widthPercent;
            offsetX += layout.padding / 2;

            highShelf.setBoundsRelative(offsetX, offsetY,
                                          layout.rows[2].cells[3].widthPercent, layout.rows[2].heightPercent);
        }
	}

    // handler for button clicks
    void AudioProcessorEditor::buttonClicked(juce::Button* button) 
    {/*
        if (button == &isOn)
        {
            if (isOn.getToggleState()) {
                processor.mainPipeline->gain->gainFactor = 0.5;
                sampleRate.setText("gain is: " + std::to_string(processor.mainPipeline->gain->gainFactor));
            }
            else {
                processor.mainPipeline->gain->gainFactor = 0.01;
                sampleRate.setText("gain is: " + std::to_string(processor.mainPipeline->gain->gainFactor));
            }
        }
        else if (button == &genInfo) {
            juce::PopupMenu IRmenu;
            IRmenu.addItem(1, "Load IR");

            //IRmenu.showMenuAsync(juce::PopupMenu::Options(), ModalCallbackFunction::create(menuCallback));
        }
    */}

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

    // handler for slider interactions
    // TODO: add predelay handling
    void AudioProcessorEditor::sliderValueChanged(juce::Slider * changedSlider)
    {
        /*juce::Slider* reverbSlider = reverbParam.getSlider(changedSlider);
        juce::Slider* lowShelfSlider = lowShelf.getSlider(changedSlider);
        juce::Slider* peakingLowSlider = peakingLow.getSlider(changedSlider);
        juce::Slider* peakingHighSlider = peakingHigh.getSlider(changedSlider);
        juce::Slider* highShelfSlider = highShelf.getSlider(changedSlider);

        if(reverbSlider)
        {
            if (reverbSlider->getComponentID() == "lenIR")
            {/*
                /* TODO: find what param to change changetempo? originalSampleRate?
                genInfo.setText("bro toggled IR volume");
                processor.irPipeline->gain->gainFactor = reverbSlider->getValue();
                sampleRate.setText("ir volume is: " + std::to_string(processor.irPipeline->gain->gainFactor));
                */
            /*}            
            else if (reverbSlider->getComponentID() == "volIR")
            {
                auto param = processor.parameters.getParameter(processor.PID_IR_GAIN);

                if (!param)
                {
                    throw std::invalid_argument("Received non-float parameter for gain in Gain block");
                }

                param->beginChangeGesture();
                param->setValueNotifyingHost(reverbSlider->getValue());
                param->endChangeGesture();
            }
            else if (reverbSlider->getComponentID() == "gainOut")
            {
                auto param = processor.parameters.getParameter(processor.PID_AUDIO_OUT_GAIN);

                if (!param)
                {
                    throw std::invalid_argument("Received non-float parameter for gain in Gain block");
                }

                param->beginChangeGesture();
                param->setValueNotifyingHost(reverbSlider->getValue());
                param->endChangeGesture();
            }
            else if (reverbSlider->getComponentID() == "dryWet")
            {
                // TODO: check if value to provide is in range [0 - 100] or [0.00 - 1]
                processor.mainPipeline->dryWetMixer->wetRatio = reverbSlider->getValue();
            }
            else
            {
                // TODO: add warning/error
            }
        }
                
        else if (lowShelfSlider)
        {
            if (lowShelfSlider->getComponentID() == "paramQ")
            {
                processor.irPipeline->filters[0]->setQ(lowShelfSlider->getValue());
            }
            else if (lowShelfSlider->getComponentID() == "paramf")
            {
                processor.irPipeline->filters[0]->setFrequency(lowShelfSlider->getValue());
            }
            else if (lowShelfSlider->getComponentID() == "paramA")
            {
                processor.irPipeline->filters[0]->setGain(lowShelfSlider->getValue());
            }
            else
            {
                // TODO: add warning/error
            }
        }
                
        else if (peakingLowSlider) {
            if (peakingLowSlider->getComponentID() == "paramQ") {
                processor.irPipeline->filters[1]->setQ(peakingLowSlider->getValue());
            }
            else if (peakingLowSlider->getComponentID() == "paramf") {
                processor.irPipeline->filters[1]->setFrequency(peakingLowSlider->getValue());
            }
            else if (peakingLowSlider->getComponentID() == "paramA") {
                processor.irPipeline->filters[1]->setGain(peakingLowSlider->getValue());
            }
            else
            {
                // TODO: add warning/error
            }
        }
                
        else if (peakingHighSlider) {
            if (peakingHighSlider->getComponentID() == "paramQ") {
                processor.irPipeline->filters[2]->setQ(peakingHighSlider->getValue());
            }
            else if (peakingHighSlider->getComponentID() == "paramf") {
                processor.irPipeline->filters[2]->setFrequency(peakingHighSlider->getValue());
            }
            else if (peakingHighSlider->getComponentID() == "paramA") {
                processor.irPipeline->filters[2]->setGain(peakingHighSlider->getValue());
            }
            else
            {
                // TODO: add warning/error
            }
        }
                
        else if (highShelfSlider) {
            if (highShelfSlider->getComponentID() == "paramQ") {
                processor.irPipeline->filters[3]->setQ(highShelfSlider->getValue());
            }
            else if (highShelfSlider->getComponentID() == "paramf") {
                processor.irPipeline->filters[3]->setFrequency(highShelfSlider->getValue());
            }
            else if (highShelfSlider->getComponentID() == "paramA") {
                processor.irPipeline->filters[3]->setGain(highShelfSlider->getValue());
            }
            else
            {
                // TODO: add warning/error
            }
        }

        else if( changedSlider == &preDelay)
        {
            processor.irPipeline->preDelay->delayMs = changedSlider->getValue();
        }

        else {
            // TODO: add warning / exception / error due to lack of handling
        }*/

    }

}