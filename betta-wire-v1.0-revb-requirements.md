# Betta Wire V1.0, RevB Requirements

This document is for the recording of requirements for the Betta Wire V1.0, RevB. 

## System Requirements 
System-level requirements for the Betta Wire V1.0 RevB. This includes general constraints, such as system voltage or firmware.

| Requirement | Description | Owner |
| :-----------| :-----------| :-----|
| Klipper Firmware | Use Klipper firmware | John (Rack Robotics) |
| System Voltage | 24 VDC | John (Rack Robotics) |
| EDM Wire Diameter | 0.3 mm | John (Rack Robotics) |
| Required 3D Printer Build Area | X210 mm x Y210 mm x Z200 mm | John (Rack Robotics)

## Controls Requirements 

Control requirements for the Betta Wire V1.0 RevB. Methods used to manage and direct the behavior of the machine and devices to achieve desired outputs. 

| Requirement | Description | Owner |
| :-----------| :-----------| :-----|
| Open-Loop Motion Planning | Open-loop control of linear motion axis | John (Rack Robotics) |
| Closed-Loop Wire Tension | Maintain wire tension using closed-loop control | John (Rack Robotics) |
| Open-Loop Wire Velocity | Open-loop control of EDM wire velocity | John (Rack Robotics) | 
| Closed-Loop Feedrate Control | Adjust feedrate in response to EDM conditions | John (Rack Robotics) | 

## Kinematics

Kinematics requirements for the Betta Wire V1.0 RevB.

| Requirement | Description | Owner |
| :-----------| :-----------| :-----|
| Dual CoreXY | Independently actuated dual coreXY systems | John (Rack Robotics) | 
| User Maintenance | Ensure means of replacing wear components in gantry | John (Rack Robotics) |  
| GT2 Timing Belts | Use GT2 timing belts with glass-core | John (Rack Robotics) | 
| Timing Belt Material | EPDM Rubber w/ glass-fiber fore | John (Rack Robotics) | 

## Top Toolhead Requirements

Requirements for the top toolhead of the Betta Wire V1.0 RevB. The top toolhead sits above the water tank, on the bottom gantry. It is responsible for reporting EDM wire tension during machining, feeding new wire through the machine.

| Requirement | Description | Owner |
| :-----------| :-----------| :-----| 
| Tension Sensor | Provide means of measuring wire tension within a range of 0 - 31 N | John (Rack Robotics) |
| Belt Drive     | Use belts to manipulate EDM wire | John (Rack Robotics) |
| IP68 Motor     | Use IP68 rated motor for belt drive actuation | John (Rack Robotics) | 
| User Maintenance | Ensure means of replacing worn belts in belt drive | John (Rack Robotics) |
| Testing | Design for testing on stationary test rig | John (Rack Robotics) | 

## Bottom Toolhead Requirements

Requirements for the bottom toolhead of the Betta Wire V1.0 RevB. The bottom toolhead sits inside the water tank, on the bottom gantry. It is responsible for receiving the EDM wire from the top toolhead, after passing through the cut. 

| Requirement | Description | Owner |
| :-----------| :-----------| :-----| 
| Belt Drive     | Use belts to manipulate EDM wire | John (Rack Robotics) |
| [IP68](#Description-of-IP68-Waterproofing) Motor     | Use IP68 rated motor for belt drive actuation | John (Rack Robotics) | 
| Energizer      | Energize EDM wire with waveforms from Powercore EDM spark generator | John (Rack Robotics) |


## Description of IP68 Waterproofing 

>An IP68 rating is an Ingress Protection (IP) code that indicates the degree of protection an electrical enclosure provides against the intrusion of solid objects (like dust) and liquids (like water). The IP code is defined by the International Electrotechnical Commission [(IEC) standard 60529](https://www.iec.ch/ip-ratings).

First Digit (6): Indicates the level of protection against solid particles.
- 6: Dust-tight. No ingress of dust; complete protection against contact (dustproof).

Second Digit (8): Indicates the level of protection against liquids.
- 8: Protection against continuous immersion in water. The equipment is suitable for continuous immersion in water under conditions specified by the manufacturer (usually means it can be submerged beyond 1 meter in depth, often up to 3 meters or more, for a specified period).