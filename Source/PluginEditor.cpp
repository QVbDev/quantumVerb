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

	//==============================================================================
	AudioProcessorEditor::AudioProcessorEditor(AudioProcessor& p)
		: juce::AudioProcessorEditor(&p), processor(p)
	{
		// Make sure that before the constructor has finished, you've set the
		// editor's size to whatever you need it to be.
		
        setResizable(true, true);
        setResizeLimits(400, 300, 1920, 1080);

        /*// Displays boxed sections with text
        activateButton.setButtonText("On/Off");
        addAndMakeVisible(activateButton);

        infoGene.setButtonText("Info gene");
        addAndMakeVisible(infoGene);

        sampleRate.setButtonText("Sample Rate");
        addAndMakeVisible(sampleRate);

        graphBox.setButtonText("Graph box");
        addAndMakeVisible(graphBox);

        gainMixTimeBox.setButtonText("gain mix time box");
        addAndMakeVisible(gainMixTimeBox);

        eqBox.setButtonText("testing EQ box");
        addAndMakeVisible(eqBox);

        addAndMakeVisible(&item2);*/


        /*// used for sliders
        // basic look and feel
        qfilter.setSliderStyle(Slider::LinearHorizontal);
        qfilter1.setSliderStyle(Slider::LinearBar);
        qfilter2.setSliderStyle(Slider::IncDecButtons);
        qfilter3.setSliderStyle(Slider::Rotary);
        qfilter4.setSliderStyle(Slider::RotaryHorizontalDrag);
        qfilter5.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        

        // min, max, increment
        qfilter.setRange(0, 100, 1);
        qfilter1.setRange(0, 100, 1);
        qfilter2.setRange(0, 100, 1);
        qfilter3.setRange(0, 100, 1);
        qfilter4.setRange(0, 100, 1);
        qfilter5.setRange(0, 100, 1);
        
        // text display pos, readonly, width, height
        qfilter.setTextBoxStyle(Slider::TextBoxAbove, false, 90, 20);
        qfilter1.setTextBoxStyle(Slider::TextBoxLeft, true, 40, 20);
        qfilter2.setTextBoxStyle(Slider::TextBoxAbove, true, 40, 20);
        qfilter3.setTextBoxStyle(Slider::TextBoxBelow, true, 40, 20);
        qfilter4.setTextBoxStyle(Slider::TextBoxBelow, true, 40, 20);
        qfilter5.setTextBoxStyle(Slider::TextBoxBelow, true, 40, 20);

        // display popup on drag, on hover, parent component, hover timeout
        // parent : this -> child is slider, nullptr -> pop-up window (doesn't work in standalone)
        qfilter.setPopupDisplayEnabled(true, false, this, -1);
        qfilter1.setPopupDisplayEnabled(false, true, this, -1);
        qfilter2.setPopupDisplayEnabled(false, true, nullptr, -1);
        qfilter3.setPopupDisplayEnabled(true, true, this, -1);
        qfilter4.setPopupDisplayEnabled(true, true, this, -1);
        qfilter5.setPopupDisplayEnabled(true, true, this, -1);

        // could be used to set units
        qfilter.setTextValueSuffix(" defined text");
        
        // add to ui
        addAndMakeVisible(&qfilter);
        addAndMakeVisible(&qfilter1);
        addAndMakeVisible(&qfilter2);
        addAndMakeVisible(&qfilter3);
        addAndMakeVisible(&qfilter4);
        addAndMakeVisible(&qfilter5);*/
        
        
        isOn.setColour(isOn.tickColourId, juce::Colour(3, 169, 244));
        isOn.setColour(isOn.tickDisabledColourId, juce::Colour(204, 204, 204));
        isOn.setButtonText("On");
        isOn.setSize(getWidth()*0.15,getHeight()*0.1);
        isOn.addListener(this);
        
        addAndMakeVisible(isOn);

        //sampleRate.setText("Sample Rate");
        sampleRate.setJustification(Justification::centred);
        sampleRate.setReadOnly(true);
        addAndMakeVisible(sampleRate);

        genInfo.setText("Example text");
        genInfo.setJustification(Justification::centred);
        genInfo.setReadOnly(true);
        addAndMakeVisible(genInfo);

        addAndMakeVisible(reverbParam);

        addAndMakeVisible(lowShelf);
        addAndMakeVisible(peakingLow);
        addAndMakeVisible(peakingHigh);
        addAndMakeVisible(highShelf);       

        std::string mixerGain = "gain is: "  + std::to_string(p.mainPipeline->gain->gainFactor);
        sampleRate.setText(mixerGain);


        File knobPath = "C:/Github/QuantumReverb/Source/sample_knob.jpg";
        knobPic = juce::ImageCache::getFromFile(knobPath);
        

	}

	AudioProcessorEditor::~AudioProcessorEditor()
	{
	}

	//==============================================================================
	void AudioProcessorEditor::paint(juce::Graphics& g)
	{
		// (Our component is opaque, so we must completely fill the background with a solid colour)
		g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

		g.setColour(juce::Colours::white);
		g.setFont(15.0f);
		//g.drawFittedText("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
		
        //headerBox.drawAt(g, 0, 0, 100);
		//g.drawRoundedRectangle(0, 15, 300, 175, 2, 1);

        // get graph box relative position
        juce::Rectangle<float> pluginWindowBox(getLocalBounds().toFloat());
        auto header = pluginWindowBox.removeFromTop(pluginWindowBox.getHeight()*0.1);
        juce::Rectangle<float> EQareaBox(pluginWindowBox.removeFromBottom(pluginWindowBox.getHeight()*0.35));


        g.drawImage(knobPic,pluginWindowBox);
        
	}    

	void AudioProcessorEditor::resized()
	{
		// This is generally where you'll want to lay out the positions of any
		// subcomponents in your editor..

        /*// used to position the boxed sections and config their resize
        Rectangle<int> pluginWindow(getLocalBounds());
        auto header = pluginWindow.removeFromTop(pluginWindow.getHeight()*0.1);
        
        activateButton.setBounds(header.removeFromLeft(header.getWidth()*0.2));      
        infoGene.setBounds(header.removeFromLeft(pluginWindow.getWidth()*0.7 - activateButton.getWidth()));
        sampleRate.setBounds(header);

        eqBox.setBounds(pluginWindow.removeFromBottom(pluginWindow.getHeight()*0.35));
        graphBox.setBounds(pluginWindow.removeFromLeft(pluginWindow.getWidth()*0.7));
        gainMixTimeBox.setBounds(pluginWindow);*/
        

        /*// slider comparaison
        Rectangle<int> pluginWindow(getLocalBounds());
        Rectangle<int> topSection(pluginWindow.removeFromTop(pluginWindow.getHeight()/2).reduced(10));
        int quaterBox = topSection.getWidth()/3;
                
        qfilter.setBounds(topSection.removeFromLeft(quaterBox).reduced(15));
        qfilter1.setBounds(topSection.removeFromLeft(quaterBox).reduced(15));
        qfilter2.setBounds(topSection.reduced(15));
        
        
        qfilter3.setBounds(pluginWindow.removeFromLeft(quaterBox).reduced(15));
        qfilter4.setBounds(pluginWindow.removeFromLeft(quaterBox).reduced(15));
        qfilter5.setBounds(pluginWindow.reduced(15));*/


        juce::Rectangle<int> pluginWindowBox(getLocalBounds());
        auto header = pluginWindowBox.removeFromTop(pluginWindowBox.getHeight()*0.1);
        juce::Rectangle<int> EQareaBox(pluginWindowBox.removeFromBottom(pluginWindowBox.getHeight()*0.35));
        int EQfilterArea = pluginWindowBox.getWidth() / 4;

        isOn.setBounds(header.removeFromLeft(header.getWidth()*0.15));
        sampleRate.setBounds(header.removeFromRight(header.getWidth()*0.35));
        genInfo.setBounds(header);

        reverbParam.setBounds(pluginWindowBox.removeFromRight(pluginWindowBox.getWidth()*0.3));

        lowShelf.setBounds(EQareaBox.removeFromLeft(EQfilterArea));
        peakingLow.setBounds(EQareaBox.removeFromLeft(EQfilterArea));
        peakingHigh.setBounds(EQareaBox.removeFromLeft(EQfilterArea));
        highShelf.setBounds(EQareaBox);
               
	}
    void AudioProcessorEditor::buttonClicked (juce::Button* button) 
    {
        if (button == &isOn)
        {
            if (isOn.getToggleState()) {
                genInfo.setText("bro it's on");
                processor.mainPipeline->gain->gainFactor = 0.5;
                sampleRate.setText("gain is: " + std::to_string(processor.mainPipeline->gain->gainFactor));
            }
            else {
                genInfo.setText("Example text");
                processor.mainPipeline->gain->gainFactor = 0.01;
                sampleRate.setText("gain is: " + std::to_string(processor.mainPipeline->gain->gainFactor));
            }

        }
    }
    void AudioProcessorEditor::sliderValueChanged(juce::Slider * slider)
    {
        if (slider == &rotationKnob) {
        }
        else {

        }
    }
}