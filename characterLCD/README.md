### Character LCD schematic

![LCD](https://user-images.githubusercontent.com/57129682/164863909-dd398ad4-5329-4bd5-a633-860951d29abd.PNG)
- since this LCD module uses 5V and my Nucleo board doesn't supply 5V output, I had to set the GPIO pins in open-drain mode and use the pull-up resistors. 
- LCD_RS ~ LCD_DB7 signals are the MCU's GPIO signals and these GPIO pins must be 5V tolerant.
- the pull-up resistor values are 4.7K ohm. too low resistance can cause too much current on the bus. 
