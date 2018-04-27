//---------------------------------------------------------------------
// <copyright file="SpinParameters.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//---------------------------------------------------------------------
#include "SpinParameters.h"

namespace Microsoft
{
	int SpinParameters::getTicksBitsToDrop()
	{
		switch (this->interval)
		{
		case SpinCounterInterval::Coarse:
			return 24;
		case SpinCounterInterval::Fine:
			return 16;
		default:
			return 24;
		}
	}

	int SpinParameters::getEntropyBytes()
	{
		return entropyBytes;
	}

	int SpinParameters::getTotalBits()
	{
		int counterBits;
		switch (this->periodicity) {
		case SpinCounterPeriodicity::None:
			counterBits = 0;
			break;
		case SpinCounterPeriodicity::Short:
			counterBits = 16;
			break;
		case SpinCounterPeriodicity::Medium:
			counterBits = 24;
			break;
		case SpinCounterPeriodicity::Long:
			counterBits = 32;
			break;
		default:
			counterBits = 0;
			break;
		}
		return counterBits + this->entropyBytes * 8;
	}

	SpinParameters::SpinParameters()
	{
	}

	SpinEntropy SpinParameters::getEntropy()
	{
		return (SpinEntropy)entropyBytes;
	}

	SpinCounterInterval SpinParameters::getInterval()
	{
		return interval;
	}

	SpinCounterPeriodicity SpinParameters::getPeriodicity()
	{
		return periodicity;
	}

	void SpinParameters::setEntropy(SpinEntropy entropy)
	{
		this->entropyBytes = (int)entropy;
	}

	void SpinParameters::setInterval(SpinCounterInterval interval)
	{
		this->interval = interval;
	}

	void SpinParameters::setPeriodicity(SpinCounterPeriodicity periodicity)
	{
		this->periodicity = periodicity;
	}

	SpinParameters SpinParameters::getDefaultSpinParameters()
	{
		SpinParameters defaultParameters = SpinParameters();
		defaultParameters.setInterval(SpinCounterInterval::Coarse);
		defaultParameters.setPeriodicity(SpinCounterPeriodicity::Short);
		defaultParameters.entropyBytes = (int)SpinEntropy::Two;
		return defaultParameters;
	}


}