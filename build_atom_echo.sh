#!/bin/bash

# Load nvm
export NVM_DIR="$HOME/.nvm"
[ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh"

# Use Node.js v16
nvm use 16

# Add Python bin to PATH
export PATH=$PATH:$HOME/Library/Python/3.9/bin

# Create a temporary platformio_override.ini file
cat > platformio_override.ini << 'EOL'
# Define atom-echo environment
[env:atom-echo]
platform = espressif32@3.5.0
board = m5stack-atom
framework = arduino
build_unflags = ${common.build_unflags}
build_flags = ${common.build_flags} ${esp32.build_flags} -D WLED_RELEASE_NAME=\"M5Stack-Atom\"
  -D WLED_DISABLE_BROWNOUT_DET
  -D LEDPIN=27
  -D BTNPIN=39
  -D LOROL_LITTLEFS=1
  -D CONFIG_LITTLEFS_FOR_IDF_3_2
  -D LITTLEFS=LittleFS
  -D WLED_DISABLE_ALEXA
  -D WLED_DISABLE_HUESYNC
  -D WLED_DISABLE_INFRARED
  -D WLED_DISABLE_MQTT
  -D WLED_DISABLE_WEBSOCKETS
  -D WLED_DISABLE_LOXONE
  -D WLED_DISABLE_ADALIGHT
  -D WLED_MAX_USERMODS=4
  -D WLED_MAX_LEDS=300
  ; Audio configuration for M5Stack Atom Echo microphone
  -D AUDIOPIN=0
  -D AUDIO_REACTIVE
  -D SR_DMTYPE=1
  ; I2S microphone settings
  -D I2S_SDPIN=23
  -D I2S_WSPIN=33
  -D I2S_CKPIN=19
  -D MCLK_PIN=-1
  -D DISABLE_I2S_OUTPUT
  -D SR_SQUELCH=15
  ; Explicitly disable speaker pins
  -D SPEAKER_PIN=22
  -D DISABLE_SPEAKER=true
  -D PULL_DOWN_GPIO22=true
  -D INIT_GPIO22_LOW=true
board_build.partitions = min_spiffs.csv
lib_deps = 
  ${esp32.lib_deps}
  https://github.com/lorol/LITTLEFS.git
  kosme/arduinoFFT@2.0.1
custom_usermods = audioreactive atom_echo_disable_speaker pitch_to_color
monitor_filters = esp32_exception_decoder

# Override platformio.ini environments list to ensure atom-echo is included
[platformio]
default_envs = atom-echo
EOL

# Create a temporary fix for the LITTLEFS issue
mkdir -p .pio/libdeps/atom-echo/
cat > .pio/libdeps/atom-echo/fix_littlefs.h << 'EOL'
#ifndef FIX_LITTLEFS_H
#define FIX_LITTLEFS_H
#define LITTLEFS LittleFS
#endif
EOL

# Run PlatformIO command with clean first to ensure a fresh build
platformio run --target clean
platformio run --target upload --environment atom-echo

# Remove the temporary files
rm platformio_override.ini
rm -rf .pio/libdeps/atom-echo/fix_littlefs.h
