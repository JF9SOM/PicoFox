#pragma once

#include <Arduino.h>

// DRA818V VHF FM transceiver module driver.
// UART: 9600bps, 8N1. Frequency range: 134–174 MHz.
// PTT pin: LOW = transmit, HIGH = receive.
// PDN pin: HIGH = normal operation, LOW = power down.

// GPIO assignments (adjust to available pins before hardware assembly).
#define DRA818_TX_PIN   8u   // RP2040 TX → DRA818V RXD
#define DRA818_RX_PIN   9u   // RP2040 RX ← DRA818V TXD
#define DRA818_PTT_PIN  10u  // PTT control (LOW = TX)
#define DRA818_PDN_PIN  11u  // Power down control (HIGH = on)
#define DRA818_SQ_PIN   12u  // Squelch output from DRA818V (not used for fox TX)

#define DRA818_BAUD     9600

// Bandwidth options for AT+DMOSETGROUP.
#define DRA818_BW_25K   0   // 25 kHz (wideband FM)
#define DRA818_BW_12K5  1   // 12.5 kHz (narrowband FM, ARDF standard)

// Initialises UART and GPIO pins. Must be called before any other function.
// Returns true if the DRA818V responds to the handshake.
bool dra818_init();

// Sends AT+DMOCONNECT and waits for "+DMOCONNECT:0" acknowledgement.
// Returns true on success.
bool dra818_handshake();

// Configures the transceiver.
//   freqMHz  : transmit/receive frequency in MHz (e.g. 145.660)
//   bandwidth: DRA818_BW_25K or DRA818_BW_12K5
//   squelch  : squelch level 0–8 (0 = open squelch)
// Returns true if the module responds with "+DMOSETGROUP:0".
bool dra818_set_group(double freqMHz, uint8_t bandwidth, uint8_t squelch);

// Asserts PTT (begins transmission). PDN must be HIGH.
void dra818_ptt_on();

// Releases PTT (ends transmission).
void dra818_ptt_off();

// Powers down the DRA818V (PDN LOW). Reduces current draw when not in use.
void dra818_power_down();

// Powers up the DRA818V (PDN HIGH). Allow 200ms settling before transmitting.
void dra818_power_up();
