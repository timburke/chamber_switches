#McGehee Chamber Switches#
This code runs on an arduino board and allows communication with the SPDT analog switches controlling the
environmental chambers.

##Requirements
This code requires the following extra arduino libraries to be installed
- TODO

##Usage
The code presents a standard serial interface to the computer at 57600 baud, 8N1 (8 data bits, no parity bits, 1 stop bit).
When using a serial terminal to communicate with the board (for testing, etc.), disable local echoing of characters and configure
the enter key to send the CR (Carriage Return, \r) code, not NL (New Line, \n) or '\r\n'.

Hitting return should bring up a command prompt.  Currently supported commands are described below

###Supported Commands
- sub2no <substrate_number> <yes|no>	
	sub2no connects every pin (except the 4 ground pins) on the substrate indicated to the NO bus if the argument yes is passed and disconnects all of the pins from the NO bus if no is passed.
	substrate_number should be an integer from 1-40 corresponding with the numbering scheme of the chamber.  
	yes means connect the substrate to the NO bus
	no means disconnect the substrate from the NO bus
- pin2no <substrate_number> <pin_number>
	pin2no connects a single pin on the indicated substrate to the NO bus.  It does not currently have the ability to disconnect a single pin, use 'sub2no <substrate> no' to disconnect the entire substrate.  The pins are numbered from 1 to 14 sequentially from the corner of the substrate.  Which pin corresponds to pin 1 must be determined empirically since it depends on how the internal wiring was done and how you're oriented relative to the chamber.  It should be consistent from substrate to substrate though.
- initstatus
	initstatus reports if there were any problems initializing the substrates to their normally closed state. If there were no problems, it reports "All substrates initialized correctly."  If there was a problem initializing one or more substrates it reports the first error it
	encountered with the string: "Initialization error.  Last error occured on substrate <substrate_number>"

##Important Notes
- The mapping of substrate number to actual substrate needs to be checked.  I entered it using the hand-drawn diagram-like object that George gave to me.  Given how confusing it was, the odds of every substrate number in the program mapping exactly with the numbers drawn on the chamber is probably unlikely.  You can figure out which substrate is which by using the sub2no function [this is mainly why it's there] and testing for connectivity between a nonground pin on that substrate and the NO bus pin.
- The i2c bus is about at its capacitance limit.  I2C, for bus arbitration purposes, does not allow devices to drive the clock or data lines high, but rather they are pulled high by a resistor.  The value of this resistor combined with the total capacitance of every device on the bus, sets the rise time of this signal.  It must be < 1/10 (I think) of the width of each clock and data pulse to avoid errors in reading the data.  With 40 substrate controller ICs on the bus, we are at or slightly over the recommended limit for bus capacitance (400 pF).  Thus an oscilloscope should be used to test the rise and fall times of the clock and data signals and make sure that they are within the i2c spec.  The value of the pull-up resistor on the lines can be lowered to decrease the signal rise-time but this is also can increase the signal fall-time so there's a tradeoff.  