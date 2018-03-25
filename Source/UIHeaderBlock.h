/*
  ==============================================================================

    GUIHeaderBlock.h

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "PluginProcessor.h"
#include "UIBlock.h"

namespace reverb
{

    class UIHeaderBlock : public UIBlock
    {
    public:
        //==============================================================================
        UIHeaderBlock(AudioProcessor& processor);

        //==============================================================================
        using Ptr = std::unique_ptr<UIHeaderBlock>;

        //==============================================================================
        void paint(juce::Graphics&) override;
        void resized() override;
        void addListener(juce::ComboBox::Listener* pluginEditor);
        juce::ComboBox* getComboBox(juce::ComboBox* clickedComboBox);

        int previousSelectedIR;

    protected:
        //==============================================================================
        using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
        using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

        //==============================================================================
        juce::TextButton isOn;
        juce::Label isOnLabel;
        std::unique_ptr<ButtonAttachment> isOnAttachment;

        juce::Label irChoiceLabel;
        juce::ComboBox irChoice;

        juce::TextButton sampleRate;
        juce::Label sampleRateLabel;

    private:
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UIHeaderBlock)
    };

}
