#pragma once

#include "JuceHeader.h"

namespace reverb
{

    class UIBlock : public juce::Component
    {
    public:
        //==============================================================================
        UIBlock(int numComponents, int numComponentsPerRow);

        //==============================================================================
        using Ptr = std::unique_ptr<UIBlock>;

        //==============================================================================
        std::vector<juce::Rectangle<int>> getComponentCells(juce::Rectangle<int> bounds);

    protected:
        //==============================================================================
        juce::TextButton frame;

        //==============================================================================
        int numComponents;
        int numComponentsPerRow;

    private:
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UIBlock)
    };

}