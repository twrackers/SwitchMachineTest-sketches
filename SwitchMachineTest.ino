#include <Wire.h>
//#include <Streaming.h>

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

// Toggle switch machines on 1 sec / 2 sec cycle.
Pulser toggleTimer(1000, 2000);

// Sequencer for stepping through I2C peripherals in sequence.
Sequencer sequencer(updateInterval, DIM(I2C_ADDR));
//int mostRecentStep;

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
bool toMain = true;

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
//  Serial.begin(115200);
}

void loop()
{
  if (toggleTimer.update()) {
    if (toMain == toggleTimer.read()) {
      // Toggle for other route...
      toMain = !toMain;
      // ... and send a move command to all switch machines.
      sequencer.start();
//      mostRecentStep = -1;
    }
  }

  int currentStep = sequencer.read();
  if (sequencer.update() 
    && (currentStep >= 0)
    /*&& (currentStep != mostRecentStep)*/) {
    digitalWrite(LED_BUILTIN, HIGH);
    byte address = I2C_ADDR[currentStep];
    byte command = toMain ? eMain : eDiv;
    for (byte chan = 0; chan < DIM(CHANNEL); ++chan) {
      send(address, command | CHANNEL[chan]);
    }
    digitalWrite(LED_BUILTIN, LOW);
//    Serial << currentStep << endl;
//    mostRecentStep = currentStep;
  }
    
  // Has the refresh pushbutton been pressed?
  if (pbRefresh.update()) {
    // Send a refresh command to the controllers.
    digitalWrite(LED_BUILTIN, HIGH);
    for (byte addr = 0; addr < DIM(I2C_ADDR); ++addr) {
      send(I2C_ADDR[addr], eRefresh);
    }
    digitalWrite(LED_BUILTIN, LOW);
  }

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
