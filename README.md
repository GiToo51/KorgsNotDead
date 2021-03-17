# Korg'sNotDead project

**Arduino Mega 2560** program to replace a defective **Korg Concert C25** into a great Midi controller

After trying to repair a Korg Concert C25 with no sound, I ended up using an Arduino Mega 2560 I had for fun to transform this keayboard into a Midi Controller.

![Final](/ressources/extern.jpg)

![Opened](/ressources/opened.jpg)

# Features
Because arduino is slow, velocity curve is precomputed and the scan runs as fast as possible. Cf: generateVelocityCurve.rb
This runs somewhere around half a milissecond for a complete scan, that's fast enouth to have a great velocity precision.

### Keybord
88 Keys, mapped into 15x(6x2) matrix.
So that means 15 half Octaves with 2 switch per key to measure velocity.

First tried if that can work

![Prototype](/ressources/prototype.jpg)

Then debugging wirering order problems.

![Debugging](/ressources/debug.jpg)

Then, so many optimisations occurs befors this very efficient final version.

### LEDS
TODO...

### Menu
TODO...

### Pedals
Internal: 2 simple switches. 3 independent wire.
External: ExtA & ExtB, OnOff and Analog reading

# Audio card
As the mixer part is dead from the original card, but the power ampifier is still working and seems to be some great quality, I tried wirering the line output of the soudcard to the input of this card. After cleaning wirering to have noiseless sound, I figured out that I missed some bass. A friend is going to help me on this as I'M not an analog signal expert.

# Midi sockets
To keep clean sockets, I cut the midi part with sockets & few components, into the original card to isolate electricaly midi cables from the arduino card.
Wired to hte IDE cable.

# Internal wirering
I don't like soldering 40wires, so I used an old IDE 40pins cable with a male male connecter, to wire everything.
Arduino Mega have 36 pins, so there are 2 free wire on each side, 2 analogs and Serial 1 in and out for midi sockets.
I had to do few tricks to use all these pins on the Arduino: Cf into comments in the code.

![Wireing](/ressources/wirering.jpg)

# Debugging

Midi IO use Serial1 native serial port. Serial on the usb port is not initialized, bug can be enabled for debugging purpose.

You can start the code in debug mode on the file you want, and debugs messages are sent to the usb serial port Serial0.
