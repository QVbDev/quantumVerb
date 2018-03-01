#pragma once

#include "JuceHeader.h"

namespace reverb
{

    class UIBlock : public juce::Component
    {
    public:
        UIBlock(int numComponents)
            : numComponents(numComponents)
        {
        }

        std::vector<juce::Rectangle<int>> getComponentCells(juce::Rectangle<int> bounds, double padding)
        {
            int outerHeight = bounds.getHeight();
            int outerWidth = bounds.getWidth();

            // Build cells
            std::vector<juce::Rectangle<int>> cells(numComponents, juce::Rectangle<int>(getLocalBounds()));

            int numCols = 2;
            int numRows = std::ceil((float)cells.size() / (float)numCols);

            int numOddCells = (cells.size() % numCols);

            int outerCellPaddingY = (outerHeight / numRows) * padding;
            int innerCellPaddingY = (outerHeight / numRows) * padding / 2;
            int outerCellPaddingX = (outerWidth / numCols) * padding;
            int innerCellPaddingX = (outerWidth / numCols) * padding / 2;

            for (int idx = 0; idx < cells.size(); ++idx)
            {
                int row = idx / numCols;
                int col = idx % numCols;

                // Top & bottom margins
                int marginTop = 20 + row * outerHeight / numRows;
                int marginBottom = (numRows - 1 - row) * outerHeight / numRows;

                // Left & right margins
                int marginLeft = col * outerWidth / numCols;
                int marginRight = (numCols - 1 - col) * outerWidth / numCols;

                if (numOddCells != 0 &&
                    row == numRows - 1)
                {
                    // Centre cells in last row if odd
                    int marginPerMissingCell = outerWidth / (2 * numCols);
                    marginLeft += (numCols - numOddCells) * marginPerMissingCell;
                    marginRight -= (numCols - numOddCells) * marginPerMissingCell;
                }

                // Add cell padding
                if (row == 0)
                {
                    marginTop += outerCellPaddingY;
                    marginBottom += innerCellPaddingY;
                }
                else if (row == numRows - 1)
                {
                    marginTop += innerCellPaddingY;
                    marginBottom += outerCellPaddingY;
                }
                else
                {
                    marginTop += innerCellPaddingY;
                    marginBottom += innerCellPaddingY;
                }

                if (col == 0)
                {
                    marginLeft += outerCellPaddingX;
                    marginRight += innerCellPaddingX;
                }
                else if (col == numCols - 1)
                {
                    marginLeft += innerCellPaddingX;
                    marginRight += outerCellPaddingX;
                }
                else
                {
                    marginLeft += innerCellPaddingX;
                    marginRight += innerCellPaddingX;
                }

                // Calculate cell bounds
                cells[idx].removeFromTop(marginTop);
                cells[idx].removeFromBottom(marginBottom);
                cells[idx].removeFromLeft(marginLeft);
                cells[idx].removeFromRight(marginRight);
            }

            return cells;
        }

    protected:
        int numComponents;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UIBlock)
    };

}