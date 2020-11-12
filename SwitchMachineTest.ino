#include <Wire.h>

#include <OneShot.h>
#include <Pulse.h>
#include <PushButton.h>

#include "SwitchMachineCmds.h"

// GPIO pin assignments
const byte pinRefresh = 12;   // "refresh" pushbutton, input
const byte pinReset   = 11;   // "reset" pushbutton, input
const byte pinToggle  = 10;   // "toggle" pushbutton, input

// I2C address of attached SwitchMachineController
const byte I2C_ADDR = 0x30;

// The onboard LED, when triggered, will flash for 200 msec.
Pulse flash(LED_BUILTIN, HIGH, 200);

// Momentary pushbuttons are connected to the input pins, so that
// each pin is pulled to GROUND when its button is pressed.
// Multiple presses are locked out if they occur within 500 msec
// after the first.
PushButton pbRefresh(pinRefresh, LOW, 500);
PushButton pbReset(pinReset, LOW, 500);
PushButton pbToggle(pinToggle, LOW, 500);

// Define the current state of the two turnouts.  When the 
// switch machine controller on the bus boots, it will set its
// switch machines to their main routes, so we set the local
// state to match.
bool toMain = true;

// Transmit byte to specific I2C address.
void send(const byte addr, const byte b)
{
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
  // Has the toggle pushbutton been pressed?
  if (pbToggle.update()) {
    // Toggle the state...
    toMain = !toMain;
    // ... and send a move command to both switch machines.
    send(I2C_ADDR, (toMain ? eMain : eDiv) | eChan0);
    send(I2C_ADDR, (toMain ? eMain : eDiv) | eChan1);
    // Trigger the LED flash.
    flash.trigger();
  }

  // Has the refresh pushbutton been pressed?
  if (pbRefresh.update()) {
    // Send a refresh command to the controller.
    send(I2C_ADDR, eRefresh);
    // Trigger the LED flash.
    flash.trigger();
  }

  // Has the reset pushbutton been pressed?
  if (pbReset.update()) {
    // Send a reset command to the controller.
    send(I2C_ADDR, eReset);
    // Trigger the LED flash.
    flash.trigger();
  }

  // Update the flash.
  flash.update();
}
