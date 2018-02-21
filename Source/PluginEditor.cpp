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
        isOn.setSize(getWidth()*0.15,getHeight()*0.1);
        isOn.addListener(this);        
        addAndMakeVisible(isOn);

        // sample rate box config
        auto sampleRateTemp = std::to_string(p.getSampleRate() / 1000);
        sampleRate.setText("Sample Rate: " + sampleRateTemp.substr(0,4) + "k");
        sampleRate.setJustification(Justification::centred);
        sampleRate.setReadOnly(true);
        addAndMakeVisible(sampleRate);

        // IR file box config
        std::size_t pos = 0;
        std::string path = p.irPipeline->irFilePath;
        genInfo.setButtonText("IR file: " + path );
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
        preDelay.setRange(0, 100, 1);
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
        juce::Rectangle<int> pluginWindowBox(getLocalBounds());
        auto header = pluginWindowBox.removeFromTop(pluginWindowBox.getHeight()*0.1);
        juce::Rectangle<int> EQareaBox(pluginWindowBox.removeFromBottom(pluginWindowBox.getHeight()*0.35));
        int EQfilterArea = pluginWindowBox.getWidth() / 5;

        isOn.setBounds(header.removeFromLeft(header.getWidth()*0.15));
        sampleRate.setBounds(header.removeFromRight(header.getWidth()*0.35));
        genInfo.setBounds(header);

        reverbParam.setBounds(pluginWindowBox.removeFromRight(pluginWindowBox.getWidth()*0.3));

        preDelay.setBounds(EQareaBox.removeFromLeft(EQfilterArea));
        lowShelf.setBounds(EQareaBox.removeFromLeft(EQfilterArea));
        peakingLow.setBounds(EQareaBox.removeFromLeft(EQfilterArea));
        peakingHigh.setBounds(EQareaBox.removeFromLeft(EQfilterArea));
        highShelf.setBounds(EQareaBox);              
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
                processor.irPipeline->filters[0]->Q = lowShelfSlider->getValue();
            }
            else if (lowShelfSlider->getComponentID() == "paramf") {
                processor.irPipeline->filters[0]->freq = lowShelfSlider->getValue();
            }
            else {
                processor.irPipeline->filters[0]->gainFactor = lowShelfSlider->getValue();
            }
        }
                
        else if (peakingLowSlider) {
            if (peakingLowSlider->getComponentID() == "paramQ") {
                processor.irPipeline->filters[0]->Q = peakingLowSlider->getValue();
            }
            else if (peakingLowSlider->getComponentID() == "paramf") {
                processor.irPipeline->filters[0]->freq = peakingLowSlider->getValue();
            }
            else {
                processor.irPipeline->filters[0]->gainFactor = peakingLowSlider->getValue();
            }
        }
                
        else if (peakingHighSlider) {
            if (peakingHighSlider->getComponentID() == "paramQ") {
                processor.irPipeline->filters[0]->Q = peakingHighSlider->getValue();
            }
            else if (peakingHighSlider->getComponentID() == "paramf") {
                processor.irPipeline->filters[0]->freq = peakingHighSlider->getValue();
            }
            else {
                processor.irPipeline->filters[0]->gainFactor = peakingHighSlider->getValue();
            }
        }
                
        else if (highShelfSlider) {
            if (highShelfSlider->getComponentID() == "paramQ") {
                processor.irPipeline->filters[0]->Q = highShelfSlider->getValue();
            }
            else if (highShelfSlider->getComponentID() == "paramf") {
                processor.irPipeline->filters[0]->freq = highShelfSlider->getValue();
            }
            else {
                processor.irPipeline->filters[0]->gainFactor = highShelfSlider->getValue();
            }
        }

        else {
            // TODO: add warning / exception / error due to lack of handling
        }

    }

}