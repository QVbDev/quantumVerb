/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <string.h>

using namespace juce;

float FILTER_BOX_RELATIVE_WIDTH = 0.2f;
float FILTER_BOX_RELATIVE_HEIGHT = 0.35f;
float HEADER_BOX_RELATIVE_HEIGHT = 0.10f;
float SAMPLE_RATE_BOX_RELATIVE_WIDTH = 0.35f;
float ON_BUTTON_RELATIVE_WIDTH = 0.15f;
float INFO_BUTTON_RELATIVE_WIDTH = 0.5f;
float REVERB_BOX_RELATIVE_HEIGHT = 0.55F;

namespace reverb
{
	AudioProcessorEditor::AudioProcessorEditor(AudioProcessor& p)
		: juce::AudioProcessorEditor(&p), processor(p)
	{
		// Make sure that before the constructor has finished, you've set the
		// editor's size to whatever you need it to be.
		
        setResizable(true, true);
        setResizeLimits(400, 300, 1920, 1080);

        // on/off button config
        isOn.setColour(isOn.tickColourId, juce::Colour(3, 169, 244));
        isOn.setColour(isOn.tickDisabledColourId, juce::Colour(204, 204, 204));
        isOn.setButtonText("On");
        isOn.addListener(this);        
        addAndMakeVisible(isOn);

        // sample rate box config
        auto sampleRateTemp = std::to_string(p.getSampleRate() / 1000);
        sampleRate.setText("Sample Rate: " + sampleRateTemp.substr(0,4) + "k");
        sampleRate.setJustification(Justification::centred);
        sampleRate.setReadOnly(true);
        addAndMakeVisible(sampleRate);

        // TODO: display IR file
        // IR file box config
        //std::size_t pos = 0;
        //std::string path = p.irPipeline->irFilePath;
        //genInfo.setButtonText("IR file: " + path );
        genInfo.addListener(this);
        addAndMakeVisible(genInfo);

        // display right-side sliders
        addAndMakeVisible(reverbParam);

        // display filter sliders
        addAndMakeVisible(lowShelf);
        addAndMakeVisible(peakingLow);
        addAndMakeVisible(peakingHigh);
        addAndMakeVisible(highShelf);       

        // predelay slider config
        preDelay.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        preDelay.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
        preDelay.setRange(0, 1000, 1);
        preDelay.addListener(this);
        addAndMakeVisible(preDelay);

        // predelay label config
        preDelayLabel.setText("Predelay", juce::NotificationType::dontSendNotification);
        preDelayLabel.setJustificationType(juce::Justification::centredBottom);
        preDelayLabel.attachToComponent(&preDelay,false);

        // make plugin editor sensitive to slider events
        reverbParam.addListener(this);
        lowShelf.addListener(this);
        highShelf.addListener(this);
        peakingHigh.addListener(this);
        peakingLow.addListener(this);
        
	}

	AudioProcessorEditor::~AudioProcessorEditor()
	{
	}

	void AudioProcessorEditor::paint(juce::Graphics& g)
	{
		// (Our component is opaque, so we must completely fill the background with a solid colour)
		g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

		g.setColour(juce::Colours::white);
		g.setFont(15.0f);
		//g.drawFittedText("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
		
        //headerBox.drawAt(g, 0, 0, 100);
		//g.drawRoundedRectangle(0, 15, 300, 175, 2, 1);

	}    

    // sets the layout of displayed components
	void AudioProcessorEditor::resized()
	{
        isOn.setBoundsRelative(0, 0, ON_BUTTON_RELATIVE_WIDTH, HEADER_BOX_RELATIVE_HEIGHT);
        isOn.setTopLeftPosition(0, 0);
        
        genInfo.setBoundsRelative(0, 0, INFO_BUTTON_RELATIVE_WIDTH, HEADER_BOX_RELATIVE_HEIGHT);
        genInfo.setTopLeftPosition(isOn.getRight(), 0);
        
        sampleRate.setBoundsRelative(0, 0, SAMPLE_RATE_BOX_RELATIVE_WIDTH, HEADER_BOX_RELATIVE_HEIGHT);
        sampleRate.setTopLeftPosition(genInfo.getRight(), 0);        

        reverbParam.setBoundsRelative(0, 0, SAMPLE_RATE_BOX_RELATIVE_WIDTH, REVERB_BOX_RELATIVE_HEIGHT);
        reverbParam.setTopRightPosition(sampleRate.getRight(),sampleRate.getBottom());
        
        preDelay.setBoundsRelative(0, 0, FILTER_BOX_RELATIVE_WIDTH, FILTER_BOX_RELATIVE_HEIGHT);
        preDelay.setTopLeftPosition(0, reverbParam.getBottom());

        lowShelf.setBoundsRelative(0, 0, FILTER_BOX_RELATIVE_WIDTH, FILTER_BOX_RELATIVE_HEIGHT);
        lowShelf.setTopLeftPosition(preDelay.getRight(), reverbParam.getBottom());

        peakingLow.setBoundsRelative(0, 0, FILTER_BOX_RELATIVE_WIDTH, FILTER_BOX_RELATIVE_HEIGHT);
        peakingLow.setTopLeftPosition(lowShelf.getRight(), reverbParam.getBottom());

        peakingHigh.setBoundsRelative(0, 0, FILTER_BOX_RELATIVE_WIDTH, FILTER_BOX_RELATIVE_HEIGHT);
        peakingHigh.setTopLeftPosition(peakingLow.getRight(), reverbParam.getBottom());

        highShelf.setBoundsRelative(0, 0, FILTER_BOX_RELATIVE_WIDTH, FILTER_BOX_RELATIVE_HEIGHT);
        highShelf.setTopLeftPosition(peakingHigh.getRight(), reverbParam.getBottom());
	}

    // handler for button clicks
    void AudioProcessorEditor::buttonClicked (juce::Button* button) 
    {
        /*
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
        */
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

    // handler for slider interactions
    // TODO: add predelay handling
    void AudioProcessorEditor::sliderValueChanged(juce::Slider * changedSlider)
    {
        juce::Slider* reverbSlider = reverbParam.getSlider(changedSlider);
        juce::Slider* lowShelfSlider = lowShelf.getSlider(changedSlider);
        juce::Slider* peakingLowSlider = peakingLow.getSlider(changedSlider);
        juce::Slider* peakingHighSlider = peakingHigh.getSlider(changedSlider);
        juce::Slider* highShelfSlider = highShelf.getSlider(changedSlider);
        if(reverbSlider){
            if (reverbSlider->getComponentID() == "lenIR") {
                /* TODO: find what param to change changetempo? originalSampleRate?
                genInfo.setText("bro toggled IR volume");
                processor.irPipeline->gain->gainFactor = reverbSlider->getValue();
                sampleRate.setText("ir volume is: " + std::to_string(processor.irPipeline->gain->gainFactor));
                */
            }            
            else if (reverbSlider->getComponentID() == "volIR") {
                processor.irPipeline->gain->gainFactor = reverbSlider->getValue();
            }
            else if (reverbSlider->getComponentID() == "gainOut") {
                processor.mainPipeline->gain->gainFactor = reverbSlider->getValue();
            }
            else {
                // TODO: check if value to provide is in range [0 - 100] or [0.00 - 1]
                processor.mainPipeline->dryWetMixer->wetRatio = reverbSlider->getValue();
            }
        }
                
        else if (lowShelfSlider) {
            if (lowShelfSlider->getComponentID() == "paramQ") {
                processor.irPipeline->filters[0]->setQ(lowShelfSlider->getValue());
            }
            else if (lowShelfSlider->getComponentID() == "paramf") {
                processor.irPipeline->filters[0]->setFrequency(lowShelfSlider->getValue());
            }
            else {
                processor.irPipeline->filters[0]->setGain(lowShelfSlider->getValue());
            }
        }
                
        else if (peakingLowSlider) {
            if (peakingLowSlider->getComponentID() == "paramQ") {
                processor.irPipeline->filters[0]->setQ(peakingLowSlider->getValue());
            }
            else if (peakingLowSlider->getComponentID() == "paramf") {
                processor.irPipeline->filters[0]->setFrequency(peakingLowSlider->getValue());
            }
            else {
                processor.irPipeline->filters[0]->setGain(peakingLowSlider->getValue());
            }
        }
                
        else if (peakingHighSlider) {
            if (peakingHighSlider->getComponentID() == "paramQ") {
                processor.irPipeline->filters[0]->setQ(peakingHighSlider->getValue());
            }
            else if (peakingHighSlider->getComponentID() == "paramf") {
                processor.irPipeline->filters[0]->setFrequency(peakingHighSlider->getValue());
            }
            else {
                processor.irPipeline->filters[0]->setGain(peakingHighSlider->getValue());
            }
        }
                
        else if (highShelfSlider) {
            if (highShelfSlider->getComponentID() == "paramQ") {
                processor.irPipeline->filters[0]->setQ(highShelfSlider->getValue());
            }
            else if (highShelfSlider->getComponentID() == "paramf") {
                processor.irPipeline->filters[0]->setFrequency(highShelfSlider->getValue());
            }
            else {
                processor.irPipeline->filters[0]->setGain(highShelfSlider->getValue());
            }
        }

        else if( changedSlider == &preDelay)
        {
            processor.irPipeline->preDelay->delayMs = changedSlider->getValue();
        }

        else {
            // TODO: add warning / exception / error due to lack of handling
        }

    }

}