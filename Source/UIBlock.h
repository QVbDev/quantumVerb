#pragma once

#include "JuceHeader.h"

namespace reverb
{

    class UIBlock : public juce::GroupComponent
    {
    public:
        //==============================================================================
        UIBlock(int numComponents, int numComponentsPerRow,
            const juce::String& componentName = juce::String(), const juce::String &labelText = juce::String());

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