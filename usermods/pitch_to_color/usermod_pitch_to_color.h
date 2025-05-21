#pragma once

#include "wled.h"

/*
 * Custom usermod that changes LED colors based on the detected pitch from the microphone
 * 
 * The colors are mapped to musical notes as follows:
 * F  - Red
 * C  - Red/Orange
 * G  - Orange
 * D  - Yellow
 * A  - Green
 * E  - Light Blue
 * B  - Turquoise
 * F# - Blue
 * Db - Light Purple
 * Ab - Dark Purple
 * Eb - Violet
 * Bb - White
 */

// Forward declaration of the AudioReactive usermod ID
#define USERMOD_ID_AUDIOREACTIVE 0

// Define note frequency ranges (in Hz)
#define NOTE_F_LOW   174.61f  // F3
#define NOTE_F_HIGH  185.00f  // between F3 and F#3
#define NOTE_F_SHARP_LOW  185.00f  // F#3
#define NOTE_F_SHARP_HIGH 196.00f  // between F#3 and G3
#define NOTE_G_LOW   196.00f  // G3
#define NOTE_G_HIGH  207.65f  // between G3 and G#3
#define NOTE_A_FLAT_LOW  207.65f  // G#3/Ab3
#define NOTE_A_FLAT_HIGH 220.00f  // between Ab3 and A3
#define NOTE_A_LOW   220.00f  // A3
#define NOTE_A_HIGH  233.08f  // between A3 and A#3
#define NOTE_B_FLAT_LOW  233.08f  // A#3/Bb3
#define NOTE_B_FLAT_HIGH 246.94f  // between Bb3 and B3
#define NOTE_B_LOW   246.94f  // B3
#define NOTE_B_HIGH  261.63f  // between B3 and C4
#define NOTE_C_LOW   261.63f  // C4
#define NOTE_C_HIGH  277.18f  // between C4 and C#4
#define NOTE_D_FLAT_LOW  277.18f  // C#4/Db4
#define NOTE_D_FLAT_HIGH 293.66f  // between Db4 and D4
#define NOTE_D_LOW   293.66f  // D4
#define NOTE_D_HIGH  311.13f  // between D4 and D#4
#define NOTE_E_FLAT_LOW  311.13f  // D#4/Eb4
#define NOTE_E_FLAT_HIGH 329.63f  // between Eb4 and E4
#define NOTE_E_LOW   329.63f  // E4
#define NOTE_E_HIGH  349.23f  // between E4 and F4

// Define colors for each note
#define COLOR_F         0xFF0000  // Red
#define COLOR_C         0xFF4000  // Red/Orange
#define COLOR_G         0xFF8000  // Orange
#define COLOR_D         0xFFFF00  // Yellow
#define COLOR_A         0x00FF00  // Green
#define COLOR_E         0x00FFFF  // Light Blue
#define COLOR_B         0x40FFFF  // Turquoise
#define COLOR_F_SHARP   0x0000FF  // Blue
#define COLOR_D_FLAT    0x8080FF  // Light Purple
#define COLOR_A_FLAT    0x800080  // Dark Purple
#define COLOR_E_FLAT    0xFF00FF  // Violet
#define COLOR_B_FLAT    0xFFFFFF  // White

class PitchToColor : public Usermod {
  private:
    // Variables
    bool enabled = true;
    bool noteDetected = false;
    float currentPitch = 0.0f;
    uint32_t currentColor = COLOR_C;  // Default color
    unsigned long lastUpdate = 0;
    
    // Reference to the AudioReactive usermod
    Usermod *audioReactive = nullptr;

    // Get the color for a specific frequency
    uint32_t getColorForFrequency(float frequency) {
      // Check which note range the frequency falls into
      if (frequency >= NOTE_F_LOW && frequency < NOTE_F_HIGH) return COLOR_F;
      if (frequency >= NOTE_F_SHARP_LOW && frequency < NOTE_F_SHARP_HIGH) return COLOR_F_SHARP;
      if (frequency >= NOTE_G_LOW && frequency < NOTE_G_HIGH) return COLOR_G;
      if (frequency >= NOTE_A_FLAT_LOW && frequency < NOTE_A_FLAT_HIGH) return COLOR_A_FLAT;
      if (frequency >= NOTE_A_LOW && frequency < NOTE_A_HIGH) return COLOR_A;
      if (frequency >= NOTE_B_FLAT_LOW && frequency < NOTE_B_FLAT_HIGH) return COLOR_B_FLAT;
      if (frequency >= NOTE_B_LOW && frequency < NOTE_B_HIGH) return COLOR_B;
      if (frequency >= NOTE_C_LOW && frequency < NOTE_C_HIGH) return COLOR_C;
      if (frequency >= NOTE_D_FLAT_LOW && frequency < NOTE_D_FLAT_HIGH) return COLOR_D_FLAT;
      if (frequency >= NOTE_D_LOW && frequency < NOTE_D_HIGH) return COLOR_D;
      if (frequency >= NOTE_E_FLAT_LOW && frequency < NOTE_E_FLAT_HIGH) return COLOR_E_FLAT;
      if (frequency >= NOTE_E_LOW && frequency < NOTE_E_HIGH) return COLOR_E;
      
      // If frequency is outside our defined ranges, return white
      return 0xFFFFFF;
    }
    
    // Get the note name for a specific frequency (for debugging)
    const char* getNoteNameForFrequency(float frequency) {
      if (frequency >= NOTE_F_LOW && frequency < NOTE_F_HIGH) return "F";
      if (frequency >= NOTE_F_SHARP_LOW && frequency < NOTE_F_SHARP_HIGH) return "F#";
      if (frequency >= NOTE_G_LOW && frequency < NOTE_G_HIGH) return "G";
      if (frequency >= NOTE_A_FLAT_LOW && frequency < NOTE_A_FLAT_HIGH) return "Ab";
      if (frequency >= NOTE_A_LOW && frequency < NOTE_A_HIGH) return "A";
      if (frequency >= NOTE_B_FLAT_LOW && frequency < NOTE_B_FLAT_HIGH) return "Bb";
      if (frequency >= NOTE_B_LOW && frequency < NOTE_B_HIGH) return "B";
      if (frequency >= NOTE_C_LOW && frequency < NOTE_C_HIGH) return "C";
      if (frequency >= NOTE_D_FLAT_LOW && frequency < NOTE_D_FLAT_HIGH) return "Db";
      if (frequency >= NOTE_D_LOW && frequency < NOTE_D_HIGH) return "D";
      if (frequency >= NOTE_E_FLAT_LOW && frequency < NOTE_E_FLAT_HIGH) return "Eb";
      if (frequency >= NOTE_E_LOW && frequency < NOTE_E_HIGH) return "E";
      return "Unknown";
    }

  public:
    // Constructor
    PitchToColor() {}

    // Initialize the usermod
    void setup() {
      // Nothing to initialize
    }

    // Initialize the usermod
    void setup() {
      // Find the AudioReactive usermod
      for (uint8_t i = 0; i < usermods.getModCount(); i++) {
        if (usermods.getMod(i)->getId() == USERMOD_ID_AUDIOREACTIVE) {
          audioReactive = usermods.getMod(i);
          DEBUG_PRINTLN(F("Found AudioReactive usermod"));
          break;
        }
      }
      
      if (!audioReactive) {
        DEBUG_PRINTLN(F("AudioReactive usermod not found! PitchToColor will not work."));
      }
    }

    // Update the usermod state
    void loop() {
      if (!enabled || !audioReactive) return;
      
      // Only update every 50ms to avoid flickering
      if (millis() - lastUpdate < 50) return;
      lastUpdate = millis();
      
      // Get the current pitch from the AudioReactive usermod
      JsonDocument doc;
      JsonObject json = doc.to<JsonObject>();
      audioReactive->addToJson(json);
      
      if (!json.containsKey("audioFreq") || !json.containsKey("audioMagnitude")) return;
      
      currentPitch = json["audioFreq"].as<float>();
      float magnitude = json["audioMagnitude"].as<float>();
      
      // Only change color if we have a strong enough signal
      if (magnitude > 10.0f) {
        currentColor = getColorForFrequency(currentPitch);
        noteDetected = true;
        
        // Apply the color to all LEDs
        for (int i = 0; i < strip.getLengthTotal(); i++) {
          strip.setPixelColor(i, currentColor);
        }
        
        // Debug output
        DEBUG_PRINTF("Detected pitch: %.2f Hz, Note: %s, Color: #%06X\n", 
                    currentPitch, getNoteNameForFrequency(currentPitch), currentColor);
      } else {
        noteDetected = false;
      }
    }

    // Add to JSON API
    void addToJsonInfo(JsonObject& root) {
      JsonObject user = root["u"];
      if (user.isNull()) user = root.createNestedObject("u");

      JsonArray usermod = user.createNestedArray("Pitch to Color");
      usermod.add(enabled ? "Enabled" : "Disabled");
      
      if (enabled && noteDetected) {
        // Display detected note information
        String noteInfo = "Note: " + String(getNoteNameForFrequency(currentPitch)) + " (" + String(currentPitch, 1) + " Hz)"; 
        usermod.add(noteInfo);
      }
    }

    // Add configuration options to the WLED web interface
    void addToConfig(JsonObject& root) {
      JsonObject top = root.createNestedObject("pitchToColor");
      top["enabled"] = enabled;
    }

    // Read configuration options from the WLED web interface
    bool readFromConfig(JsonObject& root) {
      JsonObject top = root["pitchToColor"];
      if (top.isNull()) return false;
      
      if (top["enabled"].is<bool>()) {
        enabled = top["enabled"].as<bool>();
      }
      return true;
    }

    // Add HTML to the WLED web interface
    void addToWebUI() {
      // Add settings to the WLED web interface
      String html = F(
        "<div class='usermod' id='pitchToColor'>"
        "<label class='control-label'>Pitch to Color</label>"
        "<div class='btn-group'>"
        "<div class='btn btn-block' onclick='togglePitchToColor()'>"
        "<span id='pitchToColor-toggle'>Enable</span>"
        "</div>"
        "</div>"
        "<div class='info'>"
        "<p>This effect changes the LED colors based on the detected pitch from the microphone.</p>"
        "<p><b>Color mapping:</b><br>"
        "F - Red<br>"
        "C - Red/Orange<br>"
        "G - Orange<br>"
        "D - Yellow<br>"
        "A - Green<br>"
        "E - Light Blue<br>"
        "B - Turquoise<br>"
        "F# - Blue<br>"
        "Db - Light Purple<br>"
        "Ab - Dark Purple<br>"
        "Eb - Violet<br>"
        "Bb - White</p>"
        "</div>"
        "</div>"
      );
      
      String script = F(
        "function togglePitchToColor() {"
        "  var enabled = !requestJson.pitchToColor.enabled;"
        "  requestJson.pitchToColor.enabled = enabled;"
        "  document.getElementById('pitchToColor-toggle').innerHTML = enabled ? 'Disable' : 'Enable';"
        "  requestJson.pitchToColor.enabled = enabled;"
        "  requestJson();"
        "}"
        "function updatePitchToColor() {"
        "  document.getElementById('pitchToColor-toggle').innerHTML = lastinfo.pitchToColor.enabled ? 'Disable' : 'Enable';"
        "}"
      );
      
      // Register the HTML and JS
      registerUsermod(html, script, "pitchToColor");
    }

    // Return the unique ID of this usermod
    uint16_t getId() {
      return USERMOD_ID_PITCH_TO_COLOR;
    }
};

// Define the usermod ID
#define USERMOD_ID_PITCH_TO_COLOR 9998

// Create an instance of the usermod
PitchToColor pitchToColorUsermod;

// Register the usermod
void registerUsermods() {
  usermods.add(&pitchToColorUsermod);
}
