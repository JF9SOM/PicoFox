#include "dra818.h"

// DRA818V responds with "+DMOCONNECT:0\r\n" on successful handshake,
// and "+DMOSETGROUP:0\r\n" on a successful group configuration.
#define DRA818_ACK_CONNECT  "+DMOCONNECT:0"
#define DRA818_ACK_SETGROUP "+DMOSETGROUP:0"

// Waits up to timeoutMs for the expected string to appear in the UART stream.
static bool waitForResponse(const char* expected, uint32_t timeoutMs = 2000) {
  String response = "";
  uint32_t deadline = millis() + timeoutMs;
  while (millis() < deadline) {
    while (Serial1.available()) {
      char c = (char)Serial1.read();
      response += c;
      if (response.indexOf(expected) >= 0) return true;
    }
  }
  Serial.print("DRA818: timeout waiting for: ");
  Serial.println(expected);
  return false;
}

bool dra818_init() {
  pinMode(DRA818_PTT_PIN, OUTPUT);
  pinMode(DRA818_PDN_PIN, OUTPUT);
  pinMode(DRA818_SQ_PIN,  INPUT);

  dra818_ptt_off();
  dra818_power_up();
  delay(200);  // Allow module to stabilise after power-up.

  Serial1.setTX(DRA818_TX_PIN);
  Serial1.setRX(DRA818_RX_PIN);
  Serial1.begin(DRA818_BAUD);

  return dra818_handshake();
}

bool dra818_handshake() {
  Serial1.println("AT+DMOCONNECT");
  return waitForResponse(DRA818_ACK_CONNECT);
}

bool dra818_set_group(double freqMHz, uint8_t bandwidth, uint8_t squelch) {
  // AT+DMOSETGROUP=BW,TXF,RXF,Tx_CTCSS,SQ,Rx_CTCSS
  // CTCSS 0000 = no tone. TX and RX frequency set identically (simplex).
  char cmd[64];
  snprintf(cmd, sizeof(cmd),
    "AT+DMOSETGROUP=%d,%.4f,%.4f,0000,%d,0000",
    bandwidth, freqMHz, freqMHz, squelch);
  Serial1.println(cmd);
  return waitForResponse(DRA818_ACK_SETGROUP);
}

void dra818_ptt_on() {
  digitalWrite(DRA818_PTT_PIN, LOW);
}

void dra818_ptt_off() {
  digitalWrite(DRA818_PTT_PIN, HIGH);
}

void dra818_power_down() {
  digitalWrite(DRA818_PDN_PIN, LOW);
}

void dra818_power_up() {
  digitalWrite(DRA818_PDN_PIN, HIGH);
}
