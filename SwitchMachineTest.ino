#include <Wire.h>

#include <OneShot.h>
#include <Pulse.h>
#include <Pulser.h>
#include <PushButton.h>

#include "Sequencer.h"
#include "SwitchMachineCmds.h"

// GPIO pin assignments
const byte pinRefresh = 12;   // "refresh" pushbutton, input
const byte pinReset   = 11;   // "reset" pushbutton, input
const byte pinToggle  = 10;   // "toggle" pushbutton, input

#define DIM(x) ((sizeof(x)) / (sizeof(*(x))))

// I2C addresses of attached SwitchMachineControllers
const byte I2C_ADDR[] = {0x32, 0x33};

// Channel codes
const byte CHANNEL[] = {eChan0, eChan1, eChan2, eChan3};

// Time (msec) a controller will need to update all its channels.
const int updateInterval = 50 * DIM(CHANNEL);

// Toggle switch machines on 1 sec / 1 sec cycle.
Pulser toggleTimer(1000, 1000);

// Sequencer for stepping through I2C peripherals in sequence.
Sequencer sequencer(updateInterval, DIM(I2C_ADDR));

// Momentary pushbuttons are connected to the input pins, so that
// each pin is pulled to GROUND when its button is pressed.
// Multiple presses are locked out if they occur within 500 msec
// after the first.
PushButton pbRefresh(pinRefresh, LOW, 500);
PushButton pbReset(pinReset, LOW, 500);
PushButton pbToggle(pinToggle, LOW, 500);

// Define the current state of four turnouts.  When the 
// switch machine controller on the bus boots, it will set its
// switch machines to their main routes, so we set the local
// state to match.
bool toDiv = false;

// Transmit byte to specific I2C address.
void send(const byte addr, const byte b)
{
  pinMode(LED_BUILTIN, OUTPUT);
  Wire.beginTransmission(addr);
  Wire.write(b);
  Wire.endTransmission();
}

void setup()
{
  // Enable I2C.
  Wire.begin();
}

void loop()
{
  // Time to change the turnout positions?
  if (toggleTimer.update()) {
    // If so, does current position match desired?
    if (toDiv != toggleTimer.read()) {
      // If not, toggle for desired route...
      toDiv = !toDiv;
      // ... and trigger sending move command to all switch machines.
      sequencer.start();
    }
  }

  // When not triggered, read() returns -1;
  // when triggered, read() returns 0 through (#steps - 1).
  int currentStep = sequencer.read();
  // Has sequencer updated, and is sequence started?
  if (sequencer.update() && (currentStep >= 0)) {
    // If so, flash the builtin LED.
    digitalWrite(LED_BUILTIN, HIGH);
    // With each step, use the I2C address in sequence.
    byte address = I2C_ADDR[currentStep];
    // Select correct command code.
    byte command = toDiv ? eDiv : eMain;
    // For selected address, send command to toggle each channel.
    for (byte chan = 0; chan < DIM(CHANNEL); ++chan) {
      send(address, command | CHANNEL[chan]);
    }
    digitalWrite(LED_BUILTIN, LOW);
  }
    
  // TODO: Add sequencer logic.
  // Has the refresh pushbutton been pressed?
  if (pbRefresh.update()) {
    // Send a refresh command to the controllers.
    digitalWrite(LED_BUILTIN, HIGH);
    for (byte addr = 0; addr < DIM(I2C_ADDR); ++addr) {
      send(I2C_ADDR[addr], eRefresh);
    }
    digitalWrite(LED_BUILTIN, LOW);
  }

  // TODO: Add sequencer logic.
  // Has the reset pushbutton been pressed?
  if (pbReset.update()) {
    // Send a reset command to the controllers.
    digitalWrite(LED_BUILTIN, HIGH);
    for (byte addr = 0; addr < DIM(I2C_ADDR); ++addr) {
      send(I2C_ADDR[addr], eReset);
    }
    digitalWrite(LED_BUILTIN, LOW);
  }
}
