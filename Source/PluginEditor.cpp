/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

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

        addAndMakeVisible(&item2);
        
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
		g.drawFittedText("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
		
        //headerBox.drawAt(g, 0, 0, 100);
		//g.drawRoundedRectangle(0, 15, 300, 175, 2, 1);
	}    

	void AudioProcessorEditor::resized()
	{
		// This is generally where you'll want to lay out the positions of any
		// subcomponents in your editor..

        Rectangle<int> pluginWindow(getLocalBounds());
        auto header = pluginWindow.removeFromTop(pluginWindow.getHeight()*0.1);
        
        activateButton.setBounds(header.removeFromLeft(header.getWidth()*0.2));      
        infoGene.setBounds(header.removeFromLeft(pluginWindow.getWidth()*0.7 - activateButton.getWidth()));
        sampleRate.setBounds(header);

        eqBox.setBounds(pluginWindow.removeFromBottom(pluginWindow.getHeight()*0.35));
        graphBox.setBounds(pluginWindow.removeFromLeft(pluginWindow.getWidth()*0.7));
        gainMixTimeBox.setBounds(pluginWindow);
        
        
        
	}

}