#!/bin/bash

# Load nvm
export NVM_DIR="$HOME/.nvm"
[ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh"

# Use Node.js v16
nvm use 16

# Add Python bin to PATH
export PATH=$PATH:$HOME/Library/Python/3.9/bin

# Run PlatformIO command
platformio run --target upload --environment atom-echo
