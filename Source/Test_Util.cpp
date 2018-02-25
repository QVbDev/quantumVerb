/*
  ==============================================================================

    Test_Util.cpp

  ==============================================================================
*/

#pragma once

bool compareValues(const float a1, const float a2) {
	float ratio = a1 / a2;

	if (ratio >= 0.99 && ratio <= 1.01)
		return true;

	else
		return false;

}