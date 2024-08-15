# Betta Wire V1.0, RevB Requirements
This document is for the recording of requirements for the Betta Wire V1.0, RevB. Requirements are organized by category, assembly, or component. All requirements must have a description and owner. Requirement headings are optimized for hyperlink referencing in other documents.  

## System Requirements
System-level requirements for the Betta Wire V1.0 RevB. This includes general constraints, such as system voltage or firmware.

| Requirement                   | Description                                                                                   | Owner                 | Importance            | Rational                                          |
| :-----------------------------| :---------------------------------------------------------------------------------------------| :---------------------| :---------------------| :-------------------------------------------------|
| Klipper Firmware              | Use Klipper firmware                                                                          | John (Rack Robotics)  | Mandatory             | [Design Decision](Design-Decisions.md##Firmware)  |
| System Voltage                | 24 VDC                                                                                        | John (Rack Robotics)  | Preferred             | Industry Standard for FDM 3D printers             | 
| EDM Wire Diameter             | 0.3 mm                                                                                        | John (Rack Robotics)  | Preferred             | [Design Decision](Design-Decisions.md##EDM-Wire)  | 

## Hardware Requirements
Requirements for hardware, such as fasteners. 
| Requirement                   | Description                                                                                   | Owner                 | Importance            | Rational                                          |
| :-----------------------------| :---------------------------------------------------------------------------------------------| :---------------------| :---------------------| :-------------------------------------------------|
| Fastener Material             | Stainless Steel 316L (2) > Stainless Steel 304                                                | John (Rack Robotics)  | Mandatory             | Resistance against corrosion in distilled water   | 
| Fasteners Count               | Minimize quantity & diversity of fasteners                                                    | John (Rack Robotics)  | Mandatory             | Minimize cost and complexity                      | 
| Fastener Style                | Metric, DIN 7991 Countersunk                                                                  | John (Rack Robotics)  | Preferred             | Flush mounting of fasteners in FDM prints         |
| Fastener Type(s)              | Pins > Countersunk M3 > Countersunk M4 > Countersunk M5 > Countersunk M6                      | John (Rack Robotics)  | Mandatory             | Minimize cost and complexity                      |

## 3D Printer Requirements
Requirements for 3D printing Betta Wire V1.0 components. The Betta Wire V1.0 is sold as a hardware kit, in the style of Voron. The plastic components are printed by the customer. This reduces kit cost significantly, and allows for user to customize their build's color scheme or materials.

> WARNING: CF-PA (carbon-fiber nylon) is not suitable for use on the Betta Wire or Wire Tool. CF-PA Filament deteriorates rapidly in water, loosing mechanical stiffness within days. Parts printed in CF-PA cannot be submerged in water. 

| Requirement                   | Description                                                                                   | Owner                 | Importance            | Rational                                          |
| :-----------------------------| :---------------------------------------------------------------------------------------------| :---------------------| :---------------------| :-------------------------------------------------|
| Minimum 3D Printer Build Area | X210 mm x Y210 mm x Z200 mm                                                                   | John (Rack Robotics)  | Mandatory             | Compatibility with common 3D printer build volumes|
| Recommended 3D Printer        | Prusa MK4/MK3, Bambu X1C, Bambu P1P                                                           | John (Rack Robotics)  | Mandatory             | Compatibility with common 3D printer build volumes|
| Recommended Filament          | [CF-PLA](https://us.store.bambulab.com/products/pla-cf?variant=41003202248840)                | John (Rack Robotics)  | Preferred             | Excellent quality, minimum warping                | 
| Alternative Filament          | [ASA](https://www.prusa3d.com/category/prusament-asa/)                                        | John (Rack Robotics)  | Preferred             | Minimal warping compared to ABS                   |

## Hand Tool Requirements 
Requirements for hand tools for user assembly. 
| Requirement                   | Description                                                                                   | Owner                 |
| :-----------------------------| :---------------------------------------------------------------------------------------------| :---------------------|
| Metric Allen Key Set          | Allen key set for assembly of metric fasteners                                                | John (Rack Robotics)  |
| Metric Drill Bits for Plastic | M5, M4, & M3 Drill bits for sizing holes in FDM 3D prints                                     | John (Rack Robotics)  |
| Hand drill                    | Small handheld drill for drill bits                                                           | John (Rack Robotics)  |
| Gloves                        | Disposable gloves for applying super-glue and corrosion inhibitor                             | John (Rack Robotics)  |

## Controls Requirements 
Control requirements for the Betta Wire V1.0 RevB. Methods used to manage and direct the behavior of the machine and devices to achieve desired outputs. 

| Requirement                   | Description                                                                                   | Owner                 | Importance            | Rational                                          |
| :-----------------------------| :---------------------------------------------------------------------------------------------| :---------------------| :---------------------| :-------------------------------------------------|
| Open-Loop Motion Planning     | Open-loop control of linear motion axis                                                       | John (Rack Robotics)  | Mandatory             | [Design Decision](Design-Decisions.md###Why-not-closed-loop-motors-for-kinematics?) |     
| Open-Loop Wire Velocity       | Open-loop control of EDM wire velocity                                                        | John (Rack Robotics)  | Preferred             | Outside of project scope                          | 
| Closed-Loop Wire Tension      | Maintain wire tension using closed-loop control                                               | John (Rack Robotics)  | Mandatory             | Improve machining reliability                     | 
| Closed-Loop Feedrate Control  | Adjust feedrate in response to EDM conditions                                                 | John (Rack Robotics)  | Mandatory             | Improve machining reliability                     | 
| EDM PSU E-Shutdown            | Automatically shutdown EDM PSU in the event of a prolonged short (PWM > short_threshold)      | John (Rack Robotics)  | Mandatory             | Safety, protection of PSU                         | 

## Kinematics
Kinematics requirements for the Betta Wire V1.0 RevB.

| Requirement                   | Description                                                                                   | Owner                 | Importance            | Rational                                          |
| :-----------------------------| :---------------------------------------------------------------------------------------------| :---------------------| :---------------------| :-------------------------------------------------|
| Dual CoreXY                   | Independently actuated dual coreXY systems                                                    | John (Rack Robotics)  | Mandatory             | [Design Decision](Design-Decisions.md##RevB-Kinematics    | 
| User Maintenance              | Ensure means of replacing wear components in gantry                                           | John (Rack Robotics)  | Mandatory             | Improve user experience                           | 
| GT2 Timing Belts              | Use GT2 timing belts with glass-core                                                          | John (Rack Robotics)  | Mandatory             | Industry standard for FDM 3D printers             | 
| Timing Belt Material          | EPDM Rubber w/ glass-fiber fore                                                               | John (Rack Robotics)  | Mandatory             | Reduce belt stretching over time                  | 

## Modules

### Overall Wire EDM Toolhead Requirements
The sender toolhead and receiver toolhead work together to control the position, tension, and velocity of the wire used for EDM cutting.

| Requirement                   | Description                                                                                   | Owner                 |
| :-----------------------------| :---------------------------------------------------------------------------------------------| :---------------------|
| Maximum Tension of Wire       |                                                                                               | John (Rack Robotics)  |
| Maximum Velocity of Wire      |                                                                                               | John (Rack Robotics)  |
| Minimum Velocity of Wire      |                                                                                               | John (Rack Robotics)  |
| Tube for Wire                 | Use 4 mm OD teflon tubing for wire interconnects                                              | John (Rack Robotics)  |
| [IP68](#P68-Waterproofing)    | Use IP68 rated motor for belt drive actuation                                                 | John (Rack Robotics)  |
| Testing                       | Design for testing on stationary test rig                                                     | John (Rack Robotics)  |


### Sender Toolhead Module Requirements
Requirements for the sender toolhead of the Betta Wire V1.0 RevB. The top toolhead sits above the water tank, on the bottom gantry. It is responsible for reporting EDM wire tension during machining, feeding new wire through the machine.

| Requirement                   | Description                                                                                   | Owner                 |
| :-----------------------------| :---------------------------------------------------------------------------------------------| :---------------------|
| Belt Drive                    | Use belts to manipulate EDM wire                                                              | John (Rack Robotics)  |
| Tension Sensor                | Use load cell for measuring wire tension                                                      | John (Rack Robotics)  |
| Energizer                     | Energize EDM wire with waveforms from Powercore EDM spark generator                           | John (Rack Robotics)  |
| Tube for Flushing             | Use 4 mm OD PU tube for providing delivering 100 PSI water for flushing                       | John (Rack Robotics)  |

### Receiver Toolhead Module Requirements
Requirements for the receiver toolhead of the Betta Wire V1.0 RevB. The bottom toolhead sits inside the water tank, on the bottom gantry. It is responsible for receiving the EDM wire from the top toolhead, after passing through the cut. 

| Requirement                   | Description                                                                                   | Owner                 |
| :-----------------------------| :---------------------------------------------------------------------------------------------| :---------------------|
| Belt Drive                    | Use belts to manipulate EDM wire                                                              | John (Rack Robotics)  |

### Waterproof Endstop Module
Requirements for waterproof endstops for homing kinematics

| Requirement                   | Description                                                                                   | Owner                 |
| :-----------------------------| :---------------------------------------------------------------------------------------------| :---------------------|
| Hall Effect Sensor            | Use a hall-effect sensor for detection of a 6 mm x 2 mm magnet on the toolhead/carriage       | John (Rack Robotics)  |

### Wire Spool Box Module

### Waste Wire Box Module

### E-Box Module

## Custom Klipper Modifications

### Closed-Loop Feedrate Control
Requirements for closed-loop feedrate control using Klipper firmware: 

>Closed-loop control is a control-scheme that continuously monitors its output and automatically adjusts its inputs to maintain the desired output, using feedback to correct any deviations from the target performance. This ensures precision and stability in achieving the set goals.

Wire EDM failures often occur because the rate of travel (feedrate) of the electrode outpaces the material-removal rate (MRR). To eliminate this failure mode, the feedrate must be adjusted in response to feedback from the Powercore V2.0 spark generator. The Powercore V2.0 provides feedback using the PWM output port (ALRT). The PWM duty cycle is mapped to the Powercore's "observed spark ignition rate", which is strongly correlated to the material-removal rate. As the material-removal rate increases, the PWM duty cycle increases.

| PWM Duty Cycle                | Description                                               | Controls Response                                                     | 
| :-----------------------------| :---------------------------------------------------------| :---------------------------------------------------------------------|
| 20%                           | No machining is occurring                                 | Increase feedrate                                                     | 
| 80%                           | Optimum material-removal rate                             | Maintain feedrate                                                     | 
| 100%                          | Short circuit; EDM wire electrode has crashed into work   | Decrease feedrate, or move backwards in program until short is clear  |

To achieve this behavior, new features must be added to Klipper: 

| Requirement                   | Description                                                                                   | Owner                 |
| :-----------------------------| :---------------------------------------------------------------------------------------------| :---------------------|
| Read PWM Duty Cycle           | Read PWM duty cycle of GPIO pin on control board, from PC-02 ALRT port                        | John (Rack Robotics)  |
| Automatic Feedrate Adjustment | Automatically increase/decrease/maintain feedrate in response to PWM duty cycle               | John (Rack Robotics)  |
| Display feedback on GUI       | Provide machine operator with PWM telemetry                                                   | John (Rack Robotics)  |
| Target Feedrate Range         | 0.1 mm/min to 64 mm/min                                                                       | John (Rack Robotics)  |
| Response Time                 | 100 ms to 1,000 ms                                                                            | John (Rack Robotics)  |
| Feedrate Acceleration         | Rate of change of feedrate must be limited                                                    | John (Rack Robotics)  | 
| Powercore V2.0 ALRT Port      | 3.3 VDC                                                                                       | John (Rack Robotics)  |
| Powercore V2.0 PWM Frequency  | 10-100 KHz                                                                                    | John (Rack Robotics)  |

### Dual Gantry Motion Section for Klipper
Requirements for new Klipper kinematics module, allowing for independent control of two coreXY gantries. The secondary coreXY system is provides two additional axis: The U-axis and the V-axis. The U-axis is parallel to the X-axis, and the V-axis is parallel to the Y-axis.

| Requirement                   | Description                                                                                   | Owner                 |
| :-----------------------------| :---------------------------------------------------------------------------------------------| :---------------------|
| Dual coreXY Motion Module     | Allow for independent control over two coreXY gantries                                        | John (Rack Robotics)  |
| Top coreXY Axis               | X-axis & Y-axis                                                                               | John (Rack Robotics)  | 
| Bottom coreUV Axis            | U-axis & V-axis                                                                               | John (Rack Robotics)  | 
| gcode example                 | G1 X69 Y420 U69 V420                                                                          | John (Rack Robotics)  |

### Wire EDM Toolhead Section for Klipper
Requirements for new Klipper section for use in printer.cfg, to be used for controlling wire tension. 

| Requirement                   | Description                                                                                   | Owner                 |
| :-----------------------------| :---------------------------------------------------------------------------------------------| :---------------------|
| edm_wire_toolhead             | Section name                                                                                  | John (Rack Robotics)  | 
| sensor_type                   | load cell                                                                                     | John (Rack Robotics)  | 
| sensor_range                  | 5 kg                                                                                          | John (Rack Robotics)  | 
| amplifier                     | HX711                                                                                         | John (Rack Robotics)  | 
| control                       | PID                                                                                           | John (Rack Robotics)  | 
| edm_wire_sender               | PWM pin for sender DC motor control                                                           | John (Rack Robotics)  | 
| edm_wire_receiver             | PWM pin for receiver DC motor control                                                         | John (Rack Robotics)  | 
| mainsail telemetry            | provide load cell value (newtons of force) to user in mainsail GUI (for front end engineer)   | John (Rack Robotics)  | 

### Wire EDM Macros

**Macro for "wire_EDM_toolhead_diagnostic"**
Macro to test the maximum tension that the belt drive can provide. At the end of the test, the maximum value, in newtons of force, is printed for the user. 

- inform user 'conducting wire EDM toolhead diagnostic" 
- deactivate sender motor (PWM = 0)
- activate receiver motor (PWM = 255)
- measure load cell reading 
- print load cell value to user in newtons of force

**Macro for "wire_EDM_safe_pause"**
Macro for pausing the program during a wire cutting operation, without moving the tools. 

- inform user "pausing wire EDM"
- print current position of all axis to user
- deactivate EDM PSU to stop high-voltage (disable digital GPIO pin assigned to EDM PSU)
- stop motion 
- lock motors 
- do not home (to avoid crashing wire)

**Macro for "wire_EDM_toolhead_loading"**
Macro for loading wire into the belt drive systems. 

- print message to ask user to load EDM wire into sender
- turn on wire sender DC motor at specified PWM level 
- turn on wire receiver DC motor at specified PWM level 
- wait until the tension detected by the load cell increases, and then begin PID control loop, to make sure the wire is loaded
- stop DC motors 

**Macro for "wire_EDM_toolhead_unloading"**
Macro for unloading wire into the belt drive systems. 

- ask user to cut EDM wire 
- provide section for custom gcode commands
- stop DC motors

## IP68 Waterproofing

>An IP68 rating is an Ingress Protection (IP) code that indicates the degree of protection an electrical enclosure provides against the intrusion of solid objects (like dust) and liquids (like water). The IP code is defined by the International Electrotechnical Commission [(IEC) standard 60529](https://www.iec.ch/ip-ratings).

First Digit (6): Indicates the level of protection against solid particles.
- 6: Dust-tight. No ingress of dust; complete protection against contact (dustproof).

Second Digit (8): Indicates the level of protection against liquids.
- 8: Protection against continuous immersion in water. The equipment is suitable for continuous immersion in water under conditions specified by the manufacturer (usually means it can be submerged beyond 1 meter in depth, often up to 3 meters or more, for a specified period).