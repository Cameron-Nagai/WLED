#pragma once

#include "wled.h"

/*
 * This usermod explicitly disables the speaker on M5Stack Atom Echo
 * by setting the speaker pins to INPUT_PULLDOWN or OUTPUT LOW
 */
class AtomEchoDisableSpeaker : public Usermod {
  private:
    // Speaker pins on M5Stack Atom Echo
    const uint8_t AMP_DATA_PIN = 22;  // AMP DATA pin
    const uint8_t AMP_BCLK_PIN = 19;  // AMP BCLK pin

  public:
    void setup() {
      // Configure speaker pins as INPUT_PULLDOWN to prevent any signal output
      pinMode(AMP_DATA_PIN, INPUT_PULLDOWN);
      pinMode(AMP_BCLK_PIN, INPUT_PULLDOWN);
      
      // Alternative approach: set as OUTPUT and drive LOW
      // pinMode(AMP_DATA_PIN, OUTPUT);
      // digitalWrite(AMP_DATA_PIN, LOW);
      // pinMode(AMP_BCLK_PIN, OUTPUT);
      // digitalWrite(AMP_BCLK_PIN, LOW);
      
      DEBUG_PRINTLN(F("Atom Echo speaker pins disabled"));
    }

    void loop() {
      // Nothing to do in the loop
    }

    uint16_t getId() {
      return USERMOD_ID_ATOM_ECHO_DISABLE_SPEAKER;
    }

    void addToConfig(JsonObject& root) {
      // No configuration options needed
    }

    bool readFromConfig(JsonObject& root) {
      // No configuration to read
      return true;
    }
};

// Register the usermod
void registerUsermods() {
  usermods.add(new AtomEchoDisableSpeaker());
}
