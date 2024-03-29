# SwitchMachineTest #

The *SwitchMachineTest* sketch runs on an Arduino-class microcontroller, and communicates via I<sup>2</sup>C with one or more Arduino-type microcontrollers running the [*SwitchMachineController*](https://github.com/twrackers/SwitchMachineController-sketches) sketch.  The controller sketch is written to run on an Adafruit Pro Trinket or ItsyBitsy 32u4, and supports up to four switch machines, numbered 0 through 3.  For this sketch, all switch machines on all controllers, if connected, are operated together.  You can find a sample track layout with two turnouts in the `extras` directory [here](https://github.com/twrackers/SwitchMachineTest-sketches/blob/main/extras/loop.png).

The test sketch toggles the controllers' outputs periodically between their 'main' and 'diverging' settings.  The sketch also supports two momentary normally-open pushbuttons connected between GPIO pins and ground.

- pin 11: Sends a RESET command to the switch machine controllers.
- pin 12: Sends a REFRESH command to the switch machine controllers.

More information about the commands recognized by the switch machine controller can be found in the *SwitchMachineController* sketch's README file at [https://github.com/twrackers/SwitchMachineController-sketches/blob/main/README.md](https://github.com/twrackers/SwitchMachineController-sketches/blob/main/README.md)

## Test Configuration ##

The test setup uses a [STEMTera™](https://stemtera.com/), a breadboard with an Arduino UNO compatible processor inside, as a test processor.  Mounted in the breadboard space on top is a pair of Adafruit Pro Trinket processors, each programmed with the [*SwitchMachineController*](https://github.com/twrackers/SwitchMachineController-sketches) sketch.  There is also one or two momentary normally-open pushbuttons between two GPIO pins on the STEMTera and a ground connection.

The STEMTera's I<sup>2</sup>C pins *SDA* and *SCL* are connected to the Pro Trinkets' *SDA* and *SCL* pins as well, forming an I<sup>2</sup>C bus with the STEMTera as the central and the Pro Trinkets as peripherals.  Each Pro Trinket uses 3 GPIO pins to set its peripheral I<sup>2</sup>C address to a value between 0x30 and 0x37.  For this setup, I used 0x30 and 0x31.

LEDs are connected to some of the 3-pin output channels of the *SwitchMachineController* processors, which are connected as [blue, red, green] to monitor [*enable*, *red*, *black*] signals respectively for each channel.

The *SwitchMachineTest* runs in a loop where it sends commands via I<sup>2</sup>C to switch all connected controllers' channels to "diverging" state for 1 second, then back to "main" state for 2 seconds, then repeats indefinitely.  By pushing either of the connected pushbuttons, "reset" and "refresh" commands can also be sent to the controllers.

## Installation ##

Instructions for installing the *SwitchMachineTest* sketch can be found in file `INSTALL.md` in this repository at [https://github.com/twrackers/SwitchMachineTest-sketches/blob/main/INSTALL.md](https://github.com/twrackers/SwitchMachineTest-sketches/blob/main/INSTALL.md).

## Dependencies ##

The *SwitchMachineTest* sketch uses the following libraries.

- *Wire* library, part of the Arduino IDE distribution
- *FIFO* library, [https://github.com/twrackers/FIFO-library](https://github.com/twrackers/FIFO-library)
- *OneShot* library, [https://github.com/twrackers/OneShot-library](https://github.com/twrackers/OneShot-library)
- *Pulse* library, [https://github.com/twrackers/Pulse-library](https://github.com/twrackers/Pulse-library)
- *Pulser* library, [https://github.com/twrackers/Pulser-library](https://github.com/twrackers/Pulser-library)
- *PushButton* library, [https://github.com/twrackers/PushButton-library](https://github.com/twrackers/PushButton-library)
- *StateMachine* library, [https://github.com/twrackers/StateMachine-library](https://github.com/twrackers/StateMachine-library)
