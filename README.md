<p align="center">
<img src="https://github.com/VU2DLE/Radioberry_Console/blob/main/Photo/Heading.png" width="75%" />
</p>

Radioberry Controller is very simple standalone console for piHPSDR, which is designed only for RadioBerry SDR project. Simple way to control piHPSDR using hardware controls mounted in front panel. Both software and hardware initial version is ready to use, but still development is progressing, new features may get added and bugs being fixed. 

My sincere thanks to Johan for the RadioBerry SDR project, John Melton for piHPSDR.
Rama VU3RDD and Deepu VU3TLD for suggestions and design reviews from the beginning of this project.

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

## Hardware
Hardware is very simple. Solder the 4 mechanical encoder and 16 push buttons switch on the top of the PCB, also place the 10K resistors.
<p align="center">
<img src="https://github.com/VU2DLE/Radioberry_Console/blob/main/Photo/PCB_Top.jpg" />
</p>
At the bottom, use appropriate PCB Receptacles Female Box Header Connector for placing the Raspberry Pi Pico. Luckily, I got cheap inexpensive connectors from local shop. Take 2 sets of, 18 way 1 row straight PCB female socket, through hole type. Carefully bend the leads 90degree and solder them.
<p align="center">
<img src="https://github.com/VU2DLE/Radioberry_Console/blob/main/Photo/PCB_Bottom.jpg" />
</p>

## Software Installation
### Step1: Flashing the firmware
Flashing the firmware in Pico is simple. Easy method is, if you hold down the BOOTSEL button when you plug in your Pico, it will appear as a drive onto which you can drag UF2 file.
The binary file: rb_controller.uf2, is available in the folder \Radioberry_Console\Software\Firmware
### Step2: Building the Application
step-by-step methods to host the application in linux
### Step3: Rebuild the piHPSDR
