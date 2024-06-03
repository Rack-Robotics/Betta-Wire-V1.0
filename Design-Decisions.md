# Betta Wire V1.0 Design Decisions 

This page aims to illustrate the design decisions behind the Betta-Wire V1.0 Rev A. This document pertains to RevA, the first revision of Betta Wire V1.0. The Betta Wire V1.0 RevB can be found in the RevB branch of the Betta Wire V1.0 repository. This page gives background information about the key design decisions of RevA only. 

## Overview 

Betta Wire V1.0 RevA is a proof-of-concept for an open-source, 3D printable 4-axis wire EDM machine. It utilizes common off-the-shelf 3D printer components such as NEMA 17 motors, 20T idlers, 16T pulleys, GT2 timing belts, 608ZZ bearings, TMC2209 and a Manta M5P control board. Components can be 3D printed out of CF-PLA on a desktop FDM 3D printer. his design is capable of achieving tolerances of +-100 microns once tuned. 

## Kinematics 

The Betta Wire V1.0 RevA is a dual core-xy machine. Each toolhead is controlled by one core-xy system: The top gantry moves the top tool, while the bottom gantry moves the bottom tool. Four motors (A, B, C, D) allow for movement in four axis (X, Y, U, V). This permits cutting angles walls, tappers, and chamfers. Mechanical power is transmitted from the C & D motors to the bottom gantry by a 304 stainless steel axel. 

![Betta-Wire Tools](Images/Betta%20Wire%201.0%20RevA%20Tools.png)

Linear rods are 304 stainless steel, rated for continuous immersion in distilled water. Linear bushings are IGUS iglide® H, which are rated for continuous immersion in water. These bushings do not require lubrication.

### Why not linear rails? 

Linear rails able to operate underwater are not readily available. To use linear rails, while maintaining a reasonable cost, requires a sub $50 dollar solution. 

### Why not closed-loop motors for kinematics? 

Closed-loop motors present a significant control systems challenge for Klipper firmware. Integration of closed-loop motor controls for A, B, C & D motors would require a complete redesign of Klipper’s architecture.  

## Motherboard 

The Manta M5P was chosen for it’s compact integration of a 32-bit microcontroller, up to five motor drivers, and a single-board computer. 

## Firmware 

The Betta Wire V1.0 uses Klipper firmware. Klipper permits rapid revisions without the need for recompiling. In addition to a host of features: Wireless connectivity, a GUI, compatibility with most 3D printer control hardware, support for advances sensors, USB, CAN, UART, I2C, and an active development community. 

## EDM Power / Spark-Generator 

The Betta Wire V1.0 RevA utilizes the Powercore V1.0 wire EDM power supply. This is an open-source EDM power supply designed and sold by Rack Robotics, Inc.

## RevB 

Revision B is the version of the Betta Wire V1.0 under development, which will ship as a hardware kit to customers. This revision of the machine is under development in the ‘RevB’ branch of this repository. 
