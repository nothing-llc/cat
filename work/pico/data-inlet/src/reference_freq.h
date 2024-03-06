/*
 * pwm reference frequency utilities for the radio. nothing fancy; just sets up
 * the reference for the external pll.
 *
 * copyright (c) 2024  catherine van west <catherine.vanwest@cooper.edu>
 */

#ifndef REFERENCE_FREQ_H
#define REFERENCE_FREQ_H

#include "pico/stdlib.h"

namespace reference_freq {

constexpr const static uint pin = 6;

void mostly_configure();
void enable();
void disable();
void set_mhz(float freq, bool start = true);
float actual_freq(float freq);

} /* ~reference_freq:: */

#endif
