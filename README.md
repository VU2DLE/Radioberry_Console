<p align="center">
<img src="https://github.com/VU2DLE/Radioberry_Console/blob/main/Photo/Heading.png" width="75%" />
</p>
Radioberry Controller is very simple standalone console for piHPSDR, which is designed only for RadioBerry SDR project. Simple way to control piHPSDR using hardware controls mounted in front panel. Both software and hardware initial version is ready to use, but still development is progressing, new features may get added and bugs being fixed.


My sincere thanks to
Johan for the RadioBerry SDR project, 
John Melton for piHPSDR
Rama VU3RDD, Deepu VU3TLD, for design reviews / suggestions

## Version V1.0 Features
* VFO controlled by single optical encoder
* AF, AGC, RX GAIN and RIT controlled by 4 mechanical encoders
* Push Switch controls for
  * Band+, Band–
  * Mode+, Mode-
  * Step+, Step–
  * Filter+, Filter–
  * Noise functions: NB, NR ANF and SNB
  * VFO functions: VFO A to B, VFO B to A, VFO swap and Lock

## How it works
Radioberry Controller (RBC) PCB and Raspberry Pi Pico (Pico) works from the Raspberry Pi (RPI) USB power. Pico connects to the RPI using a USB cable. Pico reads the user events like switch press and movement of rotary encoder. These user inputs are processed and send to Raspberry Pi via USB interface.
In RPI, software dlecomm, receives the commands via USB and send appropriate CAT commands to piHPSDR
<p align="center">
<img src="https://github.com/VU2DLE/Radioberry_Console/blob/main/Photo/How_it_works.png" width="75%" />
</p>

## How to install
