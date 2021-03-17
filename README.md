# Korg'sNotDead project

**Arduino Mega 2560** program to replace a defective **Korg Concert C25** into a great Midi controller

After trying to repair a Korg Concert C25 with no sound, I ended up using an Arduino Mega 2560 I had for fun to transform this keayboard into a Midi Controller.

![Final](/ressources/extern.jpg)

![Opened](/ressources/opened.jpg)

# Features
So many optimisations since my first version. It's now having a full key scan around 64µs !!! (600µs in the first version ;)
So this precision is now great enouth to have a great touch response.

### Keybord
88 Keys, mapped into 15x(6x2) matrix.
So that means 15 half Octaves with 2 switch per key to measure velocity.

First tried if that can work

![Prototype](/ressources/prototype.jpg)

Then, so many optimisations occurs befors this very efficient final version.

### LEDS
TODO...

### Menu
TODO... & still in development

### Touchpad
In development

### Pedals
Internal: 2 simple switches. 3 independent wire.

External: ExtA & ExtB, OnOff and Analog reading

# Audio amplifier
As the mixer part is dead from the original card, but the power ampifier is still working and seems to be some good quality,
I tried wirering the line output of the soudcard to the input of this card. After cleaning wirering to have noiseless sound, It's working great.

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
