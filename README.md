# CS145 Embedded Projects

This repository contains five projects developed for the CS 145 course at UC Irvine. These projects are primarily implemented in C using the Microchip Studio IDE, and the MCU used is the ATMEGA 32.

## Projects

1. **Project 1: LED Blinker**

   ### Description

   This project involves designing an embedded computer centered around the ATMEGA 32 microcontroller. For input, it uses a push button, and for output, it uses an LED. The C program blinks the LED on/off for as long as the push button is pressed.

   ### Features

   - **Initial Timing**: Uses instruction timing to control the LED on/off rate with the internal 1MHz clock.
   - **Revised Timing**: Uses one of the ATMEGA 32 internal timers with an external 8MHz crystal to control the blinking rate.
   - **Blinking Rate**: 500ms on and 500ms off.


2. **Project 2: Digital Clock**

   ### Description

   This project involves designing an embedded computer centered around the ATMEGA 32 microcontroller. For input, it uses a keypad, and for output, it uses an LCD. The C program implements a digital clock, displaying MM/DD/YYYY on the top row and HH:MM:SS on the bottom row, including the necessary UI to set the date/time.

   ### Features

   - **Display**: Shows date (MM/DD/YYYY) and time (HH:MM:SS) on an LCD.
   - **UI**: Provides interface to set the date and time.
    

3. **Project 3: Music Player**

   ### Description

   This project involves designing an embedded computer centered around the ATMEGA 32 microcontroller. For input, it uses a keypad, and for output, it uses an LCD and a speaker. The C program implements a music player that can play musical notes stored in its memory.

   ### Features

   - **Playback**: Plays musical notes stored in memory.
   - **Control**: Uses keypad for input and LCD for display.


4. **Project 4: Voltmeter**
   ### Description

   This project involves designing an embedded computer centered around the ATMEGA 32 microcontroller. It uses a keypad and an analog-to-digital (AD) I/O for input and an LCD for output.
   The C program implements a simple voltmeter that:

   - Takes an AD sample every 500 ms and updates the display.
   - Shows four parameters on the display:
     1. Instantaneous voltage
     2. Maximum voltage
     3. Minimum voltage
     4. Average voltage
   - Resets the minimum, maximum, and average voltage values to "----" on a button press.
   - Initiates sampling of these values on another button press.
   - Always displays instantaneous voltage from the moment the device is powered on.
   - Utilizes the maximum AD precision available.
   - Uses timer/interrupt for precise timing.

   ### Features

   1. **Instantaneous Voltage Display**: Continuously displays the current voltage.
   2. **Max, Min, and Average Voltage**: Calculates and displays these values based on sampled data.
   3. **Reset Function**: Button press to reset the max, min, and average values.
   4. **Sampling Control**: Button press to start sampling for max, min, and average values.
   5. **Precise Timing**: Uses timer/interrupt for sampling every 500 ms.

5. **Project 5: Alarm Clock**
   ### Description

   This project is an alarm clock with the following features:
   - **Basic Features**:
     - A running clock that increments time.
     - Time entering mode to set the alarm time.
     - Audible sound notification when the clock reads the user-input alarm time.

   - **Extra Features**:
     - Multiple alarm settings.
     - World clock functionality.
     - The clock keeps ticking while the alarm clock is still ringing (when using the simple alarm clock functionality, not the timer, stopwatch, or other modes).

   ### Features

   1. **Running Clock**: The clock runs continuously, keeping track of the current time.
   2. **Alarm Setting Mode**: Users can enter a mode to set the time at which the alarm should go off.
   3. **Audible Notification**: When the clock matches the set alarm time, the system emits an audible sound to notify the user.
   4. **Multiple Alarms**: Users can set multiple alarms for different times.
   5. **World Clock**: Displays the time in different time zones.
   6. **Continuous Ticking**: The clock continues to tick even when the alarm is ringing, ensuring the time is always accurate.

## Development Environment

- **IDE**: Microchip Studio
- **MCU**: ATMEGA 32

### Prerequisites

- [Microchip Studio](https://www.microchip.com/mplab/microchip-studio)
- [ATMEGA 32](https://www.microchip.com/wwwproducts/en/ATmega32)

## License

This repository is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.

## Acknowledgments

- Professor Amir M. Rahmani for the guidance and support throughout the course.
- UC Irvine for providing the resources and environment to complete these projects.
