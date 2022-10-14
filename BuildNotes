Building this project made for a really fun week. The goal was to create an item that would show off 3D printing and simple electronics in an eyecatching, interactive, and fun way. I certainly think I achieved that here.

The base of this project is Will Cogley's excellent 3D printed eye mechanism, which was chosen as a good fit for the halloween season. His original controller design was great for individual use by a skilled operator, but wasn't really built to be robust or simple to use. As such, I designed a much larger and simpler controller that could house all required parts.
The controller's basic BOM is present within the Printables link, but you wll also need wiring skills and an assortment of M3 bolts to complete it. I did not record the specifics of these, but an experienced maker should have no trouble replicating the general functionality.
Make sure to adjust the code if you are using a different slider. Mine was logarithmic, so the code currently has a function to re-linearize the values.

I've also built a mount that lifts the eyes off the table and provides a space to mount a 4xAA battery pack. Combined with the 9V battery in the controller this makes the setup very mobile, though the batteries drain in a few hours of use, so I recommend a wall power supply if possible. The base also provides a captive nut to attach to a tripod, which allows the eyes to be mounted at about eye-level, significantly increasing their visibility (and uncanny stare).

The software prioritizes turining basic control inputs into fairly realistic motion. Each servo is calibrated in code via boundaries (min/max) and a "center" value that sets the forward-center-closed position. A bit of mapping and math from there prevent most collision scenarios and allow the eyelids to react more naturally to the movement of the eyes.
I mapped my controller so that the stick set direction, the slider controlled how far the eyelids were open, the dial adjusted the vertical offset between the eyes and eyelids, and the button initiated a blink.

Switching the auto function on allows the eyes to look about and "react" on their own. Great for showcasing the movements when someone isn't at the controls. This uses a set of 5 basic routines that make heavy use of random numbers to emulate fairly realistic behaviors. I welcome further improvements!
The switch also allows a return to the manual control (must be on pin 2 or 3 for interrupt!) and the LED on the arcade button provides status indication (dim for manual, flicker for auto, bright for confirmation switching back to manual).

A simple wiring diagram can be found below. Note that all GND and +5V connections are the same (makes the illustration easier). There is also a change to the resistor in the button (68 Ohm for 5V) and the addition of a large capacitor in parallel with the battery supplying the servos. Neither are completely required, but I do recommend them.
![Wiring Diagram](Wiring%20Diagram.jpg)
