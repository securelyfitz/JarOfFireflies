# Jar of Fireflies
ATtiny-based Jar of Fireflies soldering project with inspiration and reuse from jason webb's https://github.com/jasonwebb/Jar-of-Fireflies

## Hardware Description
This project is intended to be a simple soldering project based on the ATTiny using only a handful of through-hole components. It uses 3 I/O pins to drive 6 charlieplexed LEDs, and an additional 2 I/O pins to control a capacitive sensor.

I originally intended to have the jar's metal ring be touch sensitive alone. However, for cap sensing to work, you need to be grounded, and a PCB inside a glass jar isn't exactly grounded. To account for that, there are several exposed ground pads just inside the ring - touch the ring and one of those pads, and the cap sensing should work.

## Bill of Materials:
* 3x 150 Ohm Resistor
* 2x 1K Ohm Resistor
* 1x 1M Ohm Resistor
* 1x ATTiny85 8-DIP
* 1x 20mm CR2032 holder and battery
* (optional) 1x SMT SPST-NO reset button

##Building
See the PDF for step by step images of building the pcb.

All of the components are labeled right on the board. Rev 1.0 of the board had a minor issue where R4 and R5 were swapped, if you have a Rev 1.0 board, it probably has a sticker correcting it.

You can optionally put the battery holder on the bottom of the board, to keep it out of reach of small hands. Just be sure to trim the pointy leads after soldering.

I used 30 gague wire and gullwing reverse-mount LEDs I had lying around. I soldered 2 LEDs in opposite polarity along each pair of wires. This mean I don't have to worry about the polarity when i solder it to the PCB. I did leave 6 pads for LEDs in case you want to do it the normal way.

##Software Description
This code is an Arduino sketch. I used the Sparkfun TinyAVR Programmer and follow the linked hookup guide to get it working: https://www.sparkfun.com/products/11801

The code for lighting the Charlieplexed LED's is cut and pasted from Jason Webb's project linked above. The code for sensing the capacitive buttons is based on Paul Stoffregen's updated CapacitiveSensor libarry: https://github.com/PaulStoffregen/CapacitiveSensor 

I rely heavily on global variables, and use the state and values to determine what to display on any given iteration. This allows me to responsively check the capsense button without dealing with interrupts.



