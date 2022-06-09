# SwitchMachineTest #

The *SwitchMachineTest* sketch runs on an Arduino-class microcontroller, and communicates via I<sup>2</sup>C with another Arduino running the *SwitchMachineController* sketch.  The controller sketch is written to run on an Adafruit Pro Trinket or ItsyBitsy 32u4, and supports up to four switch machines, numbered 0 through 3.  For this sketch, all four switch machines, if connected, are used together.  You can find a sample track layout with two turnouts in the `extras` directory at [https://github.com/twrackers/SwitchMachineTest-sketches/blob/main/extras/loop.png](https://github.com/twrackers/SwitchMachineTest-sketches/blob/main/extras/loop.png).

The Arduino running the test sketch requires three momentary normally-open pushbuttons connected between GPIO pins and ground.

- pin 10: Toggles all switch machines between their main and diverging routes.
- pin 11: Sends a RESET command to the switch machine controller.
- pin 12: Sends a REFRESH command to the switch machine controller.

More information about the commands recognized by the switch machine controller can be found in the *SwitchMachineController* sketch's README file at [https://github.com/twrackers/SwitchMachineController-sketches/blob/main/README.md](https://github.com/twrackers/SwitchMachineController-sketches/blob/main/README.md)

## Installation ##

Instructions for installing the *SwitchMachineTest* sketch can be found in file `INSTALL.md` in this repository at [https://github.com/twrackers/SwitchMachineTest-sketches/blob/main/INSTALL.md](https://github.com/twrackers/SwitchMachineTest-sketches/blob/main/INSTALL.md).

## Dependencies ##

The *SwitchMachineTest* sketch uses the following libraries.

- *Wire* library, part of the Arduino IDE distribution
- *OneShot* library, [https://github.com/twrackers/OneShot-library](https://github.com/twrackers/OneShot-library)
- *Pulse* library, [https://github.com/twrackers/Pulse-library](https://github.com/twrackers/Pulse-library)
- *PushButton* library, [https://github.com/twrackers/PushButton-library](https://github.com/twrackers/PushButton-library)
- *StateMachine* library, [https://github.com/twrackers/StateMachine-library](https://github.com/twrackers/StateMachine-library)
