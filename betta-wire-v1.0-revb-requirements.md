# Betta Wire V1.0, RevB Requirements

This document is for the documentation of requirements for the Betta Wire V1.0, RevB.

## System Requirements 

| Requirement | Description | Owner |
| :-----------| :-----------| :-----|
| Klipper Firmware | Use Klipper firmware | John (Rack Robotics) |
| System Voltage | 24 VDC | John (Rack Robotics) |
| EDM Wire Diameter | 0.3 mm | John (Rack Robotics) |

## Controls Requirements 

| Requirement | Description | Owner |
| :-----------| :-----------| :-----|
| Open-Loop Motion Planning | Open-loop control of linear motion axis | John (Rack Robotics) |
| Closed-Loop Wire Tension | Maintain wire tension using closed-loop control | John (Rack Robotics) |
| Open-Loop Wire Velocity | Open-loop control of EDM wire velocity | John (Rack Robotics) | 
| Closed-Loop Feedrate Control | Adjust feedrate in response to EDM conditions | John (Rack Robotics) | 

## Kinematics

| Requirement | Description | Owner |
| :-----------| :-----------| :-----|
| Dual CoreXY | Independently actuated dual coreXY systems | John (Rack Robotics) | 
| GT2 Timing Belts | USe GT2 timing belts with glass-core | John (Rack Robotics) | 

## Top Toolhead Requirements

| Requirement | Description | Owner |
| :-----------| :-----------| :-----| 
| Tension Sensor | Provide means of measuring wire tension within a range of 0 - 31 N | John (Rack Robotics) |
| Belt Drive     | Use belts to manipulate EDM wire | John (Rack Robotics) |
| IP68 Motor     | Use IP68 rated motor for belt drive actuation | John (Rack Robotics) | 
| User Maintenance | Ensure means of replacing worn belts in belt drive | John (Rack Robotics) |  

## Description of IP68 Waterproofing 

An IP68 rating is an Ingress Protection (IP) code that indicates the degree of protection an electrical enclosure provides against the intrusion of solid objects (like dust) and liquids (like water). The IP code is defined by the International Electrotechnical Commission [(IEC) standard 60529](https://www.iec.ch/ip-ratings).

First Digit (6): Indicates the level of protection against solid particles.
- 6: Dust-tight. No ingress of dust; complete protection against contact (dustproof).

Second Digit (8): Indicates the level of protection against liquids.
- 8: Protection against continuous immersion in water. The equipment is suitable for continuous immersion in water under conditions specified by the manufacturer (usually means it can be submerged beyond 1 meter in depth, often up to 3 meters or more, for a specified period).