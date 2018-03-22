/*
  ==============================================================================

    UIBlock.cpp

  ==============================================================================
*/

#include "UIBlock.h"
#include "PluginEditor.h"

namespace reverb
{
    
    UIBlock::UIBlock(int numComponents, int numComponentsPerRow,
        const juce::String& componentName, const juce::String &labelText)
        : numComponents(numComponents), numComponentsPerRow(numComponentsPerRow),
        juce::GroupComponent(componentName, labelText)
    {
        frame.setClickingTogglesState(false);
        frame.setInterceptsMouseClicks(false, true);

        addAndMakeVisible(frame);
    }

    std::vector<juce::Rectangle<int>> UIBlock::getComponentCells(juce::Rectangle<int> bounds)
    {
        int numRows = (int)std::ceil((float)numComponents / (float)numComponentsPerRow);
        int numCols = numComponentsPerRow;

        int rowHeight = (int)std::floor((float)bounds.getHeight() / (float)numRows);
        int colWidth = (int)std::floor((float)bounds.getWidth() / (float)numCols);

        int numOddCells = (numComponents % numCols);

        // Construct cells
        std::vector<juce::Rectangle<int>> cells(numComponents);
        
        int idx = 0;
        for (int i = 0; i < numRows; ++i)
        {
            auto row = bounds.removeFromTop(rowHeight);

            // If last row is not full, centre remaining cells
            if (i == numRows - 1 && numOddCells > 0)
            {
                int leftMargin = colWidth * (numComponentsPerRow - numOddCells) / 2;
                row.removeFromLeft(leftMargin);
            }

            for (int j = 0; j < numCols && idx < numComponents; ++j)
            {
                cells[idx] = row.removeFromLeft(colWidth);
                idx++;
            }
        }

        // Pad cells
        int padding = (int)(bounds.getHeight() * AudioProcessorEditor::PADDING_REL);
        int labelHeight = 20;

        for (auto& cell : cells)
        {
            cell.removeFromTop(labelHeight);
            cell.reduce(padding, padding);
        }

        return cells;
    }

}
