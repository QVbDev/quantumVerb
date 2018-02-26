/*
  ==============================================================================

    Test_Util.cpp

  ==============================================================================
*/

#pragma once

#include "Test_Util.h"

bool compareValues(const float val, const float nom) {
	float diff = std::abs(val - nom);

	if (nom != 0)
		diff /= nom;

	if (diff <= 0.01)
		return true;
	else
		return false;

}