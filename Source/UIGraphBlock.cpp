/*
  ==============================================================================

    UIGraphBlock.cpp

  ==============================================================================
*/

#include "UIGraphBlock.h"
#include "PluginEditor.h"

namespace reverb
{

    //==============================================================================
    /**
    * @brief Constructs a UIGraphBlock object
    *
    * Creates a UIGraphBlock and each of its components. Contains the IR graph which
    * is updated periodically and on notify in a separate thread.
    */
    UIGraphBlock::UIGraphBlock(AudioProcessor& processor)
        : processor(processor), juce::GroupComponent("IR Graph", "waveform - impulse response")
    {
        graphThread.reset(new std::thread(&UIGraphBlock::updateGraph, this, 100));
    }

    //==============================================================================
    /**
    * @brief Destroys a UIGraphBlock object
    *
    * Sends the stop signal to the graph thread
    */
    UIGraphBlock::~UIGraphBlock()
    {
        stopGraphThread = true;
        mustUpdate.notify_all();

        graphThread->join();
    }
    
    //==============================================================================
    void UIGraphBlock::paint(juce::Graphics& g)
    {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

        g.setColour(juce::Colours::white);
        g.setFont(15.0f);

        g.setColour(juce::Colour(0xddffffff));

        for (auto& irPlot : irChannelPlots)
        {
            g.strokePath(irPlot, juce::PathStrokeType(1.0f));
        }

        juce::Rectangle<int> bounds(getLocalBounds());
        getLookAndFeel().drawGroupComponentOutline(g, bounds.getWidth(), bounds.getHeight(), getText(),
            juce::Justification(juce::Justification::centredTop), *this);
    }

    //==============================================================================
    /**
    * @brief Manages the layout of UIGraphBlock when the block is resized
    *
    * This function defines all the relative positioning of the various UIGraphBlock
    * elements.
    */
    void UIGraphBlock::resized()
    {
        juce::Rectangle<int> bounds(getLocalBounds());
        int padding = (int)std::round(bounds.getHeight() * AudioProcessorEditor::PADDING_REL);

        bounds.reduce(4*padding, 4*padding);

        // Prepare coordinates
        float x = (float)bounds.getX();
        float width = (float)bounds.getWidth();

        float height = (float)bounds.getHeight();
        float yCentre = (float)bounds.getCentreY();
        
        // Read one sample per GRAPH_TIME_STEP_S per IR
        irChannelPlots.clear();

        for (int channel = 0; channel < processor.mainPipelines.size(); ++channel)
        {
            // Get IR and sample rate
            AudioBlock ir = processor.mainPipelines[channel]->ir;
            double sampleRate = processor.mainPipelines[channel]->sampleRate;

            int samplesPerStep = (int)std::ceil(GRAPH_TIME_STEP_S * sampleRate);
            int numSteps = (int)std::ceil(GRAPH_TOTAL_TIME_S / GRAPH_TIME_STEP_S);

            float xScale = width / numSteps;
            float yScale = height;

            // Create new plot
            irChannelPlots.emplace_back();
            auto& irPlot = irChannelPlots.back();

            float * irSamples = ir.getChannelPointer(0);

            // Preallocate memory for efficiency
            irPlot.preallocateSpace(numSteps);

            // Prologue: begin path at first sample
            irPlot.startNewSubPath(x, irSamples[0] + yCentre);

            // Retain maxima for each step
            int numStepsInIR = (int)ir.getNumSamples() / samplesPerStep;
            std::vector<float> maxValues(numStepsInIR, 0.0f);

            auto cmpAbsMax = [](float f1, float f2) { return std::abs(f1) < std::abs(f2); };

            for (int step = 0; step < numStepsInIR; ++step)
            {
                float * start = irSamples + step * samplesPerStep;
                float * end = start + samplesPerStep;

                maxValues[step] = *std::max_element(start, end, cmpAbsMax);
            }

            // Draw one sample per step
            for (int step = 0; step < numStepsInIR; step++)
            {
                irPlot.lineTo(x + step * xScale, yCentre + maxValues[step] * yScale);
            }

            // Epilogue: draw remaining 0-value points
            irPlot.lineTo(x + numStepsInIR * xScale, yCentre);
            irPlot.lineTo(x + numSteps * xScale, yCentre);
        }
    }

    //==============================================================================
    /**
     * @brief Updates IR graph
     *
     * Worker thread to update IR when condition variable is notified. Ends once
     * stopGraphThread is set to true and once notified through mustUpdate.
     *
     * @param [in] updatePeriodMs   Lazy update period to catch any missed parameter
                                    changes
     */
    void UIGraphBlock::updateGraph(int updatePeriodMs)
    {
        std::unique_lock<std::mutex> lock(updatingGraph);

        while (!stopGraphThread)
        {
            // Lock JUCE message manager (necessary for repaint() method)
            juce::Component::SafePointer<juce::Component> sp(this);

            // Run update mechanism on message manager thread
            juce::MessageManager::callAsync(
                [sp]
                {
                    if (!sp.getComponent())
                    {
                        return;
                    }

                    UIGraphBlock * gb = dynamic_cast<UIGraphBlock*>(sp.getComponent());

                    if (!gb)
                    {
                        return;
                    }

                    // Wait until parameter processing is complete
                    std::lock_guard<std::mutex> paramsLock(gb->getProcessorInstance()->updatingParams);

                    // Update graph and send repaint signal
                    std::unique_lock<std::mutex> lock(gb->updatingGraph);

                    gb->resized();
                    gb->repaint();

                    // Notify update thread about task completion
                    gb->updateComplete.notify_all();
                }
            );

            // Wait for graph update completion (time out after update period)
            updateComplete.wait_for(lock, std::chrono::milliseconds(updatePeriodMs));

            // Wait for new notification
            mustUpdate.wait_for(lock, std::chrono::milliseconds(updatePeriodMs));
        }
    }
    
    /**
     * @brief Signals thread to update graph
     *
     * Sends a notification to the worker thread through the mustUpdate condition
     * variable.
     */
    void UIGraphBlock::notifyToUpdate()
    {
        mustUpdate.notify_one();
    }

}
