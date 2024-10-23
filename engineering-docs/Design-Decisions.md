# Betta Wire V1.0 RevB Design Decisions 

This page aims to illustrate the design decisions behind the Betta-Wire V1.0 revision B. This document pertains to RevB, the second revision of Betta Wire V1.0. RevB can be found in the RevB branch of the Betta Wire V1.0 repository. This page gives background information about the key design decisions of RevB only. 

## RevB Overview 
Betta Wire V1.0 RevB is a 4-axis wire EDM machine designed for enthusiasts. It is optimized to provide a low barrier of entry for persons interested in wire EDM for prototyping, and manufacturing.

**Improvements over RevA**
- Improved user experience
- Improved precision
- Improved accuracy
- Reduced setup time 
- Closed-loop wire tension control 
- Closed-loop feedrate control

## RevB Toolheads
The toolheads are subassemblies of the Betta Wire which control the EDM wire. The top toolhead is located on the top gantry. The bottom toolhead is located on the bottom gantry. The toolheads are responsible for controlling wire location, tension, and velocity. 

Design Goals for RevB Wire EDM Toolheads 
- Measure wire tension and report value to control board
- Control wire tension via DC motor
- Control wire velocity via DC motor
- Minimize required volume to improve build space
- Support re-threading of EDM wire without removal of the Betta Wire from the aquarium tank 

The RevB toolheads are inspired by the [Papilio](https://kevinakasam.com/papilio/), a 3D printer extruder optimized for pushing 1.75 mm filament. 


## RevB Kinematics
The kinematics of the Betta Wire were designed to maximize performance while maintaining a reasonable construction cost.

### Why not linear rails?
Linear rails able to operate underwater are not readily available. To use linear rails, while maintaining a reasonable cost, requires a sub $35 dollar solution.

### Dual Core-XY
The Betta Wire V1.0 RevB is a dual core-xy machine. Each toolhead is independently controlled by one core-xy system: The four core-xy motors (A, B, C, D) allow for movement in four axis (X, Y, U, V). The top gantry (A & B) moves the top tool, while the bottom gantry (C & D) moves the bottom tool. This permits cutting angled walls, tappers, and chamfers.

### Remote Power Transmission
Mechanical power is transmitted from the C & D motors to the bottom gantry by a stainless steel axel. This axel rotates GT2 timing belt pulleys, which actuate the core-xy kinematics remotely. This scheme eliminates the need for water-proof motors, by moving the NEMA 17s above the waterline. Reducing the cost of components here permits resources to be invested into other portions of the Betta Wire, such as the tools and controls.


### Why not closed-loop motors for kinematics? 
Closed-loop motors present a significant control systems challenge for Klipper firmware. Integration of closed-loop motor controls for A, B, C & D motors would require a complete redesign of Klipper’s architecture.

### What is closed-loop feedrate control? 
Closed-loop feedrate control is the adjustment of the rate of machine travel, in response to feedback from the EDM power supply / spark-generator. When the feedrate is too slow, the machine automatically increases speed to optimize the material removal rate. This ensures that the Betta Wire is always cutting at the maximum safe speed.

## General Improvements
General improvements over RevA: 

- Endstops for automatic homing
- Suction-cup mounting of e-box, and other accessory modules for easier assembly, maintenence, and greater flexibility in tank size
- Improved kinematics to prevent binding and racking
- Stainless steel components to prevent corrosion

## Motherboard & Electronics
- The Manta M5P was chosen for it’s compact integration of a 32-bit microcontroller, up to five motor drivers, and a single-board computer. The dual coreXXY kinematics require 4 motor drivers, leaving 1 motor driver available for future expansions.   

## Firmware 
The Betta Wire V1.0 uses [Klipper](https://www.klipper3d.org) firmware. Klipper permits rapid revisions without the need for recompiling. In addition to a host of features: Wireless connectivity, a GUI, compatibility with most 3D printer control hardware, support for advances sensors, USB, CAN, UART, I2C, and an active development community.

## EDM Power / Spark-Generator 
The Betta Wire V1.0 utilizes the [Powercore V2.0 wire EDM power supply](https://github.com/Rack-Robotics/Powercore-V2.0). This is an open-source EDM power supply designed and sold by Rack Robotics, Inc.

## EDM Wire
0.2 mm Diameter hardened brass wire was selected as a compromise between maximizing tensile strength, maximizing current capacity, and minimizing cut diameter. Thinner EDM wire results in the need to remove less material for a given path. While thicker EDM wire is less prone to snap during machining. **Requires experimental verification**.

## Quality of Life Changes
Several quality of life changes and features will be implemented on RevB. Some key choices are as follows:
- Design 3D prints to easily drain and to minimize water inclusion. May be be shelled or modified with voids in print for draining
- Integrate tapped holes into aluminum extrusion frame for mounting of 3D prints without the use of t-nuts
- Integrate drain ports into 3D prints that enable draining of entire aquarium from top of machine
- Allowances in 3D printed service panels for toolless removal