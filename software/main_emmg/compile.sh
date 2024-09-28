arduino-cli core install rp2040:rp2040
arduino-cli lib install MIDIUSB 
arduino-cli lib install "MIDI Library" 
arduino-cli lib install "Adafruit GFX Library"
arduino-cli lib install "Adafruit SSD1306"
mkdir -p build ; arduino-cli compile --output-dir build

