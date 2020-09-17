
OVERVIEW

This software implements a loop which:
- Monitors CANBUS for incoming messages that control several LED outputs
- Reads and debounces several switch inputs, providing a periodic CANBUS output message
- Sends and received serial messages from PIC24

For details of usage and operation, refer to the board specification DOC0004180A.

TODO
- ENABLE input HIGH should 'turn Off' the board and put it in a low power state
    + CANBUS chip in low power state (issue SPI message + activate STANDBY)line)
    + Disable all LEDs and switch processing
    + Sleep the processor (perhaps wake-on-interrupt when enable input goes LOW)
- Test power consumption in 'OFF' mode
- 
