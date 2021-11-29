## 1. Evaluation Board
### EVAL-ADXL355Z
![evaluation board](https://user-images.githubusercontent.com/57129682/140690788-9db11b06-7ac7-4f32-a498-0e566c6349ba.png)

<br />

### Schematic
![schematic](https://user-images.githubusercontent.com/57129682/140691135-de2c512b-7206-4205-b7bd-8b2681c1cfaf.png)

<br />

### Functional Block Diagram
![diagram](https://user-images.githubusercontent.com/57129682/140726568-d8b61f95-4ebb-43cd-b4dd-54bcab22ce31.png)

<br />

### Pin Connection
![pinout1](https://user-images.githubusercontent.com/57129682/141144862-0b354b08-8b5b-4d64-a793-fa3bfb4a58a9.png)

<br />

## 2. I2C Communication
### Pin Connection
![pinout2](https://user-images.githubusercontent.com/57129682/141144880-79dde2ea-4680-4ae8-adee-db2da5898b00.png)

:exclamation: the pull-up resistance is usually 4.7K. since I don't have a 4.7K resistor, I use 1K resistors instead. 

<br />

### Device Address
- when the MISO/ASEL pin is low,  the address is 0x1D
- when the MISO/ASEL pin is high, the address is 0x53
