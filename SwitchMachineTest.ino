#include <Wire.h>

#include <FIFO.h>
#include <OneShot.h>
#include <Pulse.h>
#include <Pulser.h>
#include <PushButton.h>

#include "Sequencer.h"
#include "SwitchMachineCmds.h"

// Compute number of elements in an array.
#define DIM(x) ((sizeof(x)) / (sizeof(*(x))))

// GPIO pin assignments
const byte pinRefresh = 12;   // "refresh" pushbutton, input
const byte pinReset   = 11;   // "reset" pushbutton, input

// I2C addresses of attached SwitchMachineControllers
const byte I2C_ADDR[] = {0x30, 0x31};

// Channel codes
const byte CHANNEL[] = {eChan0, eChan1, eChan2, eChan3};

// Time (msec) a controller will need to update all its channels.
const int updateInterval = 30 * DIM(CHANNEL);

// Toggle switch machines on 1 sec / 2 sec cycle.
Pulser toggleTimer(1000, 2000);

// Sequencer for stepping through I2C peripherals in sequence.
// Update interval gives each controller time to command all its
// channels to move in sequential order before the next controller
// begins the same operation.
Sequencer sequencer(updateInterval, DIM(I2C_ADDR));

// Momentary pushbuttons are connected to the input pins, so that
// each pin is pulled to GROUND when its button is pressed.
// Multiple presses are locked out if they occur within 500 msec
// after the first.
PushButton pbRefresh(pinRefresh, LOW, 500);
PushButton pbReset(pinReset, LOW, 500);

// Define the current state of all turnouts.  When each 
// switch machine controller on the bus boots, it will set its
// switch machines to their main routes, so we set the local
// state to match.
bool toDiv = false;

// Current command being processed, 0x00 means none.
// All actual commands are non-zero.
E_CMD activeCommand = 0x00;

// Queue of switch machine commands to be sent.
FIFO commandQueue(8);

// Transmit byte to specific I2C address.
void send(const byte addr, const byte b)
{
  digitalWrite(LED_BUILTIN, HIGH);
  Wire.beginTransmission(addr);
  Wire.write(b);
  Wire.endTransmission();
  digitalWrite(LED_BUILTIN, LOW);
}

void setup()
{
  // LED will be flashed when I2C commands are being sent out.
  pinMode(LED_BUILTIN, OUTPUT);
  // Enable I2C.
  Wire.begin();
}

void loop()
{
  // Time to update toggle timer?
  if (toggleTimer.update()) {
    // Did toggle timer object change state since previous update?
    bool toDivNew = toggleTimer.read();
    if (toDiv != toDivNew) {
      // State changed, queue up matching command (eMain or eDiv).
      toDiv = toDivNew;
      commandQueue.push((byte) (toDiv ? eDiv : eMain));
    }
  }
  
  // If either pushbutton has been pushed, queue up its command code.
  if (pbRefresh.update()) {
    commandQueue.push((byte) eRefresh);
  }
  if (pbReset.update()) {
    commandQueue.push((byte) eReset);
  }

  // Get current sequence step, then update sequencer if it's
  // time to do so.  These two steps must be performed in this order;
  // the step will have been incremented when update returns true.
  int step = sequencer.read();
  bool seqUpdated = sequencer.update();
  // Action depends on which command is active (being sequenced).
  switch (activeCommand) {
    case eMain:
    case eDiv:
      {
        // These commands are sent to every channel on each controller.
        // Here we only send to the step'th controller (counting from zero)
        // if it's time for an update.
        if (seqUpdated) {
          if (step < 0) {
            // Previous sequence completed.
            activeCommand = 0x00;
          } else {
            // Sequence in progress, send commands to next controller
            // in order.
            byte address = I2C_ADDR[step];
            for (byte chan = 0; chan < DIM(CHANNEL); ++chan) {
              send(address, activeCommand | CHANNEL[chan]);
            }
          }
        }
      }
      break;
    case eReset:
    case eRefresh:
      {
        // These commands cover ALL channels on each controller,
        // so only need to be sent once per controller.
        // Here we only send to the step'th controller (counting from zero)
        // if it's time for an update.
        if (seqUpdated) {
          if (step < 0) {
            // Previous sequence completed.
            activeCommand = 0x00;
          } else {
            // Sequence in progress, send command to next controller
            // in order.
            send(I2C_ADDR[step], activeCommand);
          }
        }
      }
      break;
    default:  // activeCommand == 0x00
      {
        // No sequencing is active, check queue for next command.
        if (!commandQueue.isEmpty()) {
          // Get controller command from queue.
          activeCommand = commandQueue.pop();
          // Initiate sequencing through controllers.
          sequencer.start();
        }
      }
  }
}
