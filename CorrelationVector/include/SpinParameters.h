//---------------------------------------------------------------------
// <copyright file="SpinParameters.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//---------------------------------------------------------------------
#pragma once
#include "export.h"

namespace Microsoft {

	enum class EXPORTABLE SpinCounterInterval {
		/**
			The coarse interval drops the 24 least significant bits in time_since_epoch
			resulting in a counter that increments every 1.67 seconds. 
		*/
		Coarse,
		
		/**
			The fine interval drops the 16 least significant bits in time_since_epoch
			resulting in a counter that increments every 6.5 milliseconds.
		*/
		Fine
	};

	enum class EXPORTABLE SpinCounterPeriodicity {
		/**
			Do not store a counter as part of the spin value.
		*/
		None,
		
		/**
			The short periodicity stores the counter using 16 bits.
		*/
		Short,
		
		/**
			The medium periodicity stores the counter using 24 bits.
		*/
		Medium,
		
		/**
			The long periodicity stores the counter using 32 bits.
		*/
		Long
	};

	enum class EXPORTABLE SpinEntropy {
		/**
			Do not generate entropy as part of the spin value.
		*/
		None,
		
		/**
			Generate entropy using 8 bits.
		*/
		One,
		
		/**
			Generate entropy using 16 bits.
		*/
		Two,
		
		/**
			Generate entropy using 24 bits.
		*/
		Three,
		
		/**
			Generate entropy using 32 bits.
		*/
		Four
	};

	/**
		This class stores parameters used by the CorrelationVector Spin operator.
	*/
	class EXPORTABLE SpinParameters
	{
		friend class CorrelationVector;

	private:
		int entropyBytes;
		SpinCounterInterval interval;
		SpinCounterPeriodicity periodicity;

		int getTicksBitsToDrop();
		int getEntropyBytes();
		int getTotalBits();

	public:
		/**
			Initializes a new instance of the SpinParameters
		*/
		SpinParameters();

		/**
			Gets the number of bytes to use for entropy. Valid values from a
			minimum of 0 to a maximum of 4.
			@return The SpinEntropy representing the number of bytes
		*/
		SpinEntropy getEntropy();

		/**
			Gets the interval (proportional to time) by which the counter increments.
			@return The interval of the spin parameters
		*/
		SpinCounterInterval getInterval();

		/**
			Gets how frequently the counter wraps around to zero, as determined by the amount
			of space to store the counter.
			@return The periodicity of the spin parameters
		*/
		SpinCounterPeriodicity getPeriodicity();

		/**
			Sets the number of bytes to use for entropy. Valid values from a
			minimum of 0 to a maximum of 4. 
			@param entropy The enum representing the number of bytes to use for entropy
		*/
		void setEntropy(SpinEntropy entropy);

		/**
			Sets the interval (proportional to time) by which the counter increments.
			@param interval The interval of the spin parameters
		*/
		void setInterval(SpinCounterInterval interval);

		/**
			Sets how frequently the counter wraps around to zero, as determined by the amount
			of space to store the counter.
			@param periodicity The periodicity of the spin parameters
		*/
		void setPeriodicity(SpinCounterPeriodicity periodicity);

		/**
			Returns the default spin parameters where the Interval is Coarse, Periodicity
			is Short and Entropy is Two.
			@return A new instance of the SpinParameters with the default values set
		*/
		static SpinParameters getDefaultSpinParameters();
	};
}