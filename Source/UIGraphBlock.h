/*
  ==============================================================================

    UIGraphBlock.h

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "PluginProcessor.h"
#include "UIBlock.h"

namespace reverb
{

    //==============================================================================
    class UIGraphBlock : public juce::GroupComponent,
                         public juce::Button::Listener,
                         public juce::ComboBox::Listener,
                         public juce::Slider::Listener
    {
    public:
        //==============================================================================
        UIGraphBlock(AudioProcessor& processor);
        ~UIGraphBlock();

        //==============================================================================
        using Ptr = std::unique_ptr<UIGraphBlock>;

        //==============================================================================
        void paint(juce::Graphics&) override;
        void resized() override;

        //==============================================================================
        virtual void buttonClicked(juce::Button *) override { notifyToUpdate(); }
        virtual void comboBoxChanged(juce::ComboBox *) override { notifyToUpdate(); }
        virtual void sliderValueChanged(juce::Slider *) override { notifyToUpdate(); }

        //==============================================================================
        AudioProcessor * getProcessorInstance() { return &processor; }

        std::mutex updatingGraph;
        std::condition_variable updateComplete;

    protected:
        //==============================================================================
        AudioProcessor& processor;

        //==============================================================================
        void updateGraph(int updatePeriodMs);

        std::unique_ptr<std::thread> graphThread;
        std::atomic<bool> stopGraphThread = false;

        //==============================================================================
        void notifyToUpdate();

        std::condition_variable mustUpdate;

        //==============================================================================
        static constexpr float GRAPH_TOTAL_TIME_S = 6.0f;
        static constexpr float GRAPH_TIME_STEP_S = 1.0f / 500.0f;
        static constexpr float GRAPH_AVG_SAMPLING_STEP = 10;

        static constexpr float GRAPH_MAX_IR_VALUE = 10.0f;

        std::vector<juce::Path> irChannelPlots;

    private:
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UIGraphBlock)
    };

}