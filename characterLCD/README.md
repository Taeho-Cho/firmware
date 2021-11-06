### Character LCD schematic

![LCDsch](https://user-images.githubusercontent.com/57129682/140460947-46b99565-3525-41fb-b86f-01279c1a6813.png)
- since this LCD module uses 5V and my Nucleo board doesn't supply 5V output, I had to set the GPIO pins in open-drain mode and use the pull-up resistors. 
- LCD_RS ~ LCD_DB7 signals are the MCU's GPIO signals and these GPIO pins must be 5V tolerant.
- the pull-up resistance value is supposed to be about 4.7K. but since I don't have any 4.7K resistors, I used 1K resistors instead. less than 1K resistance can cause too much current on the bus.
