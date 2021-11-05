# DeviceDrivers


### 1. NevadaNano gas sensor schematic

![GasSensorsch](https://user-images.githubusercontent.com/57129682/140460841-024fd175-b948-4580-989b-731239a00f9b.png)
- GasSensor_RX, GasSensor_RX signals are the MCU's UART signals.

---

### 2. Character LCD schematic

![LCDsch](https://user-images.githubusercontent.com/57129682/140460947-46b99565-3525-41fb-b86f-01279c1a6813.png)
- since this LCD module uses 5V and my STM32 board doesn't supply 5V output, I had to set the GPIO pins in open-drain mode and use the pull-up resistors. 
- LCD_RS ~ LCD_DB7 signals are from the MCU's GPIO and these GPIO pins must be 5V tolerant.
- the pull-up resistance value is supposed to be about 4.7K. but since I don't have any 4.7K resistors, I used 1K resistors instead. less than 1K resistance can cause too much current on the bus.
