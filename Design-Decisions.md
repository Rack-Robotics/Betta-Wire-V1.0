# Betta Wire V1.0 RevB Design Decisions 

This page aims to illustrate the design decisions behind the Betta-Wire V1.0 revision B. This document pertains to RevB, the second revision of Betta Wire V1.0. RevB can be found in the RevB branch of the Betta Wire V1.0 repository. This page gives background information about the key design decisions of RevB only. 

## RevB Overview 

Betta Wire V1.0 RevB is a 4-axis wire EDM machine designed for enthusiasts. It is optimized to provide a low barrier of entry for persons interested in wire EDM for prototyping, and manufacturing robotics.

**Improvements over RevA**
- Improved user experience
- Improved precision
- Improved accuracy
- Reduced setup time 
- Closed-loop wire tension control 
- Closed-loop feedrate control

## RevB Kinematics 

**Dual Core-XY**: The Betta Wire V1.0 RevB is a dual core-xy machine. Each toolhead is independently controlled by one core-xy system: The four core-xy motors (A, B, C, D) allow for movement in four axis (X, Y, U, V). The top gantry (A & B) moves the top tool, while the bottom gantry (C & D) moves the bottom tool. This permits cutting angles walls, tappers, and chamfers. 

**Remote Power Transmission**: Mechanical power is transmitted from the C & D motors to the bottom gantry by a 304 stainless steel axel. This axel rotates GT2 timing belt pulleys, which actuate the core-xy kinematics remotely. 

**Cost Savings**: This scheme eliminates the need for water-proof motors, by moving the NEMA 17s above the waterline. Reducing the cost of components here permits resources to be invested into other portions of the Betta Wire, such as the tools and controls.

### Why not linear rails? 

Linear rails able to operate underwater are not readily available. To use linear rails, while maintaining a reasonable cost, requires a sub $50 dollar solution.

### Why not closed-loop motors for kinematics? 

Closed-loop motors present a significant control systems challenge for Klipper firmware. Integration of closed-loop motor controls for A, B, C & D motors would require a complete redesign of Klipper’s architecture.  

### What is closed-loop feedrate control? 

Closed-loop feedrate control is the adjustment of the rate of machine travel, in response to feedback from the EDM power supply / spark-generator. When the feedrate is too slow, the machine automatically increases speed to optimize the material removal rate. This ensures that the Betta Wire is always cutting at the maximum safe speed

### Why not closed-loop motors for kinematics? 

Closed-loop motors present a significant control systems challenge for Klipper firmware. Integration of closed-loop motor controls for A, B, C & D motors would require a complete redesign of Klipper’s architecture.  

## Motherboard 

The Manta M5P was chosen for it’s compact integration of a 32-bit microcontroller, up to five motor drivers, and a single-board computer. 

## Firmware 

The Betta Wire V1.0 uses Klipper firmware. Klipper permits rapid revisions without the need for recompiling. In addition to a host of features: Wireless connectivity, a GUI, compatibility with most 3D printer control hardware, support for advances sensors, USB, CAN, UART, I2C, and an active development community. 

## EDM Power / Spark-Generator 

The Betta Wire V1.0 utilizes the Powercore V1.0 wire EDM power supply. This is an open-source EDM power supply designed and sold by Rack Robotics, Inc.
