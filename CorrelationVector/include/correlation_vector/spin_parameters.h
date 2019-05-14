//---------------------------------------------------------------------
// <copyright file="spin_parameters.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//---------------------------------------------------------------------
#pragma once

namespace microsoft
{
enum class spin_counter_interval
{
    /**
        The coarse interval drops the 24 least significant bits in
       time_since_epoch resulting in a counter that increments every 1.67
       seconds.
    */
    coarse = 24,

    /**
        The fine interval drops the 16 least significant bits in
       time_since_epoch resulting in a counter that increments every 6.5
       milliseconds.
    */
    fine = 16
};

enum class spin_counter_periodicity
{
    /**
        Do not store a counter as part of the spin value.
    */
    none = 0,

    /**
        The short periodicity stores the counter using 16 bits.
    */
    short_length = 16,

    /**
        The medium periodicity stores the counter using 24 bits.
    */
    medium_length = 24,

    /**
        The long periodicity stores the counter using 32 bits.
    */
    long_length = 32
};

enum class spin_entropy
{
    /**
        Do not generate entropy as part of the spin value.
    */
    none = 0,

    /**
        Generate entropy using 8 bits.
    */
    one = 1,

    /**
        Generate entropy using 16 bits.
    */
    two = 2,

    /**
        Generate entropy using 24 bits.
    */
    three = 3,

    /**
        Generate entropy using 32 bits.
    */
    four = 4
};

/**
    This class stores parameters used by the CorrelationVector Spin operator.
*/
class spin_parameters
{
private:
    spin_counter_interval m_interval{spin_counter_interval::coarse};
    spin_counter_periodicity m_periodicity{
        spin_counter_periodicity::short_length};
    spin_entropy m_entropy{spin_entropy::two};

public:
    /**
    Creates the default spin_parameters where the interval is coarse,
    periodicity is short_length and entropy is two.
    */
    spin_parameters() = default;
    spin_parameters(spin_counter_interval interval,
                    spin_counter_periodicity periodicity,
                    spin_entropy entropy)
        : m_interval{interval}, m_periodicity{periodicity}, m_entropy{entropy}
    {
    }

    /**
    Gets the spin_entropy.
    @return The spin_entropy.
    */
    spin_entropy entropy() const { return m_entropy; }

    /**
    Sets the spin_entropy.
    @param entropy The spin_entropy enum representing the number of bytes to use
    for entropy.
    */
    void entropy(spin_entropy entropy) { m_entropy = entropy; }

    /**
    Gets the interval (proportional to time) by which the counter
    increments.
    @return The interval of the spin parameters.
    */
    spin_counter_interval interval() const { return m_interval; }

    /**
        Sets the interval (proportional to time) by which the counter
       increments.
        @param interval The interval of the spin parameters
    */
    void interval(spin_counter_interval interval) { m_interval = interval; }

    /**
        Gets how frequently the counter wraps around to zero, as determined by
       the amount of space to store the counter.
        @return The periodicity of the spin parameters
    */
    spin_counter_periodicity periodicity() const { return m_periodicity; }

    /**
        Sets how frequently the counter wraps around to zero, as determined by
       the amount of space to store the counter.
        @param periodicity The periodicity of the spin parameters
    */
    void periodicity(spin_counter_periodicity periodicity)
    {
        m_periodicity = periodicity;
    };

    int total_bits() const noexcept
    {
        return static_cast<int>(m_periodicity) +
               static_cast<int>(m_entropy) * 8;
    }
};
} // namespace microsoft