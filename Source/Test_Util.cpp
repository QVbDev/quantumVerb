/*
  ==============================================================================

    Test_Util.cpp

  ==============================================================================
*/

#pragma once

#include "Test_Util.h"

bool compareValues(float val, float nom, float tolerance) 
{
	float diff = std::abs(val - nom);

	if (nom != 0)
		diff /= nom;

	if (diff <= (tolerance / 100))
		return true;
	else
		return false;

}