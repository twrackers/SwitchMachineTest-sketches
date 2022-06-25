#include <Wire.h>

#include <FIFO.h>
#include <OneShot.h>
#include <Pulse.h>
#include <Pulser.h>
#include <PushButton.h>

#include "Sequencer.h"
#include "SwitchMachineCmds.h"

// Compute number of elements in an array.
#define DIM(x) ((sizeof(x))/(sizeof(*(x))))

// I2C addresses of attached SwitchMachineController modules
const byte I2C_ADDR[] = {0x30, 0x31};

// Channel codes
const byte CHANNEL[] = {eChan0, eChan1, eChan2, eChan3};

// Time (msec) a controller will need to update all its channels.
const int updateInterval = 30 * DIM(CHANNEL);

// Sequencer for stepping through I2C peripherals in sequence.
// Update interval gives each controller time to command all its
// channels to move in sequential order before the next controller
// begins the same operation.
Sequencer sequencer(updateInterval, DIM(I2C_ADDR));

// Toggle switch machines on 4 sec main / 2 sec diverging cycle.
Pulser toggleTimer(4000, 2000);

// GPIO pin assignments
const byte pinRefresh = 12;   // "refresh" pushbutton, input
const byte pinReset   = 11;   // "reset" pushbutton, input

// Momentary pushbuttons are connected to the input pins, so that
// each pin is pulled to GROUND when its button is pressed.
// Multiple presses are locked out if they occur within 500 msec
// after the first.
PushButton pbRefresh(pinRefresh, LOW, 500);
PushButton pbReset(pinReset, LOW, 500);

// Queue of switch machine commands to be sent.
FIFO commandQueue(8);

// Current command being processed, eNone means none.
// All actual commands are non-zero.
E_CMD activeCommand = eNone;

// Hold off all I2C comms until timeout after startup.
bool waiting = true;

// The current state of all turnouts.  When each switch machine
// controller on the bus boots, it will set its switch machines
// to the "refresh" state, so no matter whether the first command
// received is eMain or eDiv, the switch machine will be energized.
// The startup logic of this sketch will send an eReset command to
// all controllers, after which all turnouts will be set to the
// "main" position.
bool toMain;

// Transmit byte to specific I2C address.  LED_BUILTIN will blink.
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
  // Has startup delay expired?
  if (waiting && (millis() >= 4000)) {
    commandQueue.push((byte) eReset);
    toMain = true;
    waiting = false;
  }
  
  // If either pushbutton has been pushed, queue up its command code.
  if (pbReset.update()) {
    commandQueue.push((byte) eReset);
  }
  if (pbRefresh.update()) {
    commandQueue.push((byte) eRefresh);
  }

  // Time to update toggle timer?  Skip this if waiting in startup.
  if (toggleTimer.update() && !waiting) {
    // Did toggle timer object change state since previous update?
    bool toMainNew = toggleTimer.read();
    if (toMain != toMainNew) {
      // State changed, queue up new command (eMain or eDiv).
      commandQueue.push((byte) (toMainNew ? eMain : eDiv));
      toMain = toMainNew;
    }
  }

  // Get current sequence step, then update sequencer if it's
  // time to do so.  These two operations must be performed in this
  // order; the step will have been incremented when update returns
  // true, but we need the step number from BEFORE the increment.
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
    default:  // activeCommand == eNone == 0x00
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
