#include <MIDIUSB.h>

// Define the pin for the sustain pedal
const int sustainPedalPin = A0; // Use an analog pin for continuous mode

int lastPedalState = HIGH; // Store the last state (for switch mode)
int lastPedalValue = 0;   // Store the last analog value (for continuous mode)

void setup() {
  pinMode(sustainPedalPin, INPUT_PULLUP); // Set the sustain pedal pin as an input
}

void loop() {
  int pedalValue = analogRead(sustainPedalPin); // Read the analog value
  int midiValue = map(pedalValue, 0, 1023, 0, 127); // Map the value to MIDI range

  // Determine if the pedal is in switch or continuous mode
  if (midiValue == 0 || midiValue == 127) {
    // Likely in switch mode (digital values)
    int pedalState = (pedalValue > 512) ? LOW : HIGH; // Determine switch state

    // Send MIDI command only if the pedal state has changed
    if (pedalState != lastPedalState) {
      if (pedalState == LOW) { // Pedal pressed
        controlChange(0, 64, 127); // MIDI message for sustain on
      } else { // Pedal released
        controlChange(0, 64, 0); // MIDI message for sustain off
      }
      lastPedalState = pedalState; // Update last pedal state
    }
  } else {
    // Likely in continuous mode (analog values)
    // Send MIDI only if there is a significant change
    if (abs(midiValue - lastPedalValue) > 4) { // Adjust threshold as needed
      if (midiValue > 0) {
        controlChange(0, 64, midiValue); // Send continuous MIDI control change
      } else {
        controlChange(0, 64, 0); // Send sustain off
      }
      lastPedalValue = midiValue; // Update last value
    }
  }

  delay(10); // Small delay for debouncing
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}
