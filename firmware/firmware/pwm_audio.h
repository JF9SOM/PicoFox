#pragma once

#include <Arduino.h>
#include <hardware/pwm.h>

// PWM audio output driver for RP2040.
// Outputs 10-bit PWM at ~122 kHz carrier. An external RC low-pass filter
// converts the PWM signal to an analog voltage for the DRA818V MIC input.
//
// Recommended LPF (RC) values:
//   R = 1 kΩ, C = 33 nF  →  fc ≈ 4.8 kHz  (passes 5 kHz audio band)
//   R = 1 kΩ, C = 10 nF  →  fc ≈ 15.9 kHz (wider, more ripple on MIC)
// Use 1 kΩ / 33 nF as the primary design target.
//
// Schematic:
//   PWM_AUDIO_PIN ──[1kΩ]──┬── DRA818V MIC pin
//                          │
//                        [33nF]
//                          │
//                         GND
//
// DRA818V MIC input level: ~10 mV RMS typical for 3 kHz deviation.
// Adjust AUDIO_PWM_SCALE to calibrate deviation.

#define PWM_AUDIO_PIN     15u   // GPIO15 → LPF → DRA818V MIC
#define PWM_AUDIO_BITS    10    // 10-bit resolution (1024 levels)
#define PWM_AUDIO_WRAP    1023  // pwm_set_wrap value

// Attenuation factor applied to 16-bit PCM samples before PWM output.
// Range 1–100. Lower values reduce FM deviation on the DRA818V.
// Start at 30 and adjust empirically with a calibrated SDR.
#define AUDIO_PWM_SCALE   30

// Initialises the PWM slice for audio output.
// Call once during setup(), before any pwm_audio_write() calls.
void pwm_audio_init();

// Writes one 16-bit signed PCM sample to the PWM output.
// sample: signed 16-bit PCM value (-32768 to +32767).
// Converts to unsigned 10-bit PWM level centred at mid-scale (512).
void pwm_audio_write(int16_t sample);

// Sets PWM output to the silent (mid-scale) level.
// Call when carrier is ON but no audio is playing.
void pwm_audio_silence();
