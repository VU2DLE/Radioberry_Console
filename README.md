<p align="center">
<img src="https://github.com/VU2DLE/Radioberry_Console/blob/main/Photo/Heading.png" />
</p>

Radioberry Controller is very simple standalone console for piHPSDR, which is designed only for RadioBerry SDR project. A simple way to control piHPSDR using hardware controls mounted in front panel. Both software and hardware initial version is ready to use, but still development is progressing, new features may get added and bugs being fixed. 

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
Radioberry Controller (RBC) PCB and Raspberry Pi Pico (Pico) works from the Raspberry Pi's (RPI) USB power. Pico is the part of RBC, and connects to the RPI using a USB cable. RBC takes 5V from the USB, so no need of external DC power. RBC reads the user events like switch press and movement of rotary encoder via GPIO's of the Pico. These user inputs are processed by Pico and send to Raspberry Pi via USB interface.
In RPI, application software rbc_comm, which receives the user response commands (like clicking switches or rotation of vfo encoder) via USB and send appropriate CAT commands to piHPSDR. USB serial adapter may appear as /dev/ttyACM0 in RPI, after pluging in Pico via USB cable.
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

### Completely assembled PCB with optical rotary encoder
<p align="center">
<img src="https://github.com/VU2DLE/Radioberry_Console/blob/main/Photo/PCB_V10.jpg" />
</p>

## Software Installation
Create a directory in your raspberry pi under the /home/pi. 
Open a command window and executing the following commands:
```ruby
cd /home/pi/
mkdir rbc_main
cd rbc_main
git clone https://github.com/VU2DLE/Radioberry_Console.git
```
### Step1: Flashing the firmware
Flashing the firmware in Pico is simple. Easy method is, if you hold down the BOOTSEL button when you plug in your Pico, it will appear as a drive onto which you can drag UF2 file. The binary file: rb_controller.uf2, is available in the folder rbc_main \Radioberry_Console\Software\Firmware\bin.
### Step2: Building the Application
Go to the folder “Application”
```ruby
cd /home/pi/rbc_main/Radioberry_Console/Software/Application/
make clean
make
```
### Step3: Rebuild the piHPSDR
There are few changes I have done in the rigctl.c of the piHPSDR, which is essential. since my pull request is not yet part of the /g0orx/pihpsdr, you can use the forked pihpsdr from my github.
Simple method to rebuild the piHPSDR is by editing pihpsdr_install.sh from Johan. Follow the steps of Johan for installing Radioberry sofware.
Before executing “sudo chmod +x pihpsdr_install.sh”, open the pihpsdr_install.sh in any editor you are familiar, then remove line “git clone https://github.com/g0orx/pihpsdr.git”. Add “git clone https://github.com/VU2DLE/pihpsdr.git”. Save and close the pihpsdr_install.sh and continue remaining steps.

### Now software installation is completed.
make sure that radioberry is running and also pihpsdr. In pihpsdr, goto Menu->RIGCTL. rbc_comm program communicate piHPSDR via this TCP Port number 19090. Select Rigctl Enable tick mark and Rigctl Port Number is 19090.

Now Open a command window and execute the program
```ruby
cd /home/pi/rbc_main/Radioberry_Console/Software/Application/
./rbc_comm
```
Now you see the following output in the command window
```ruby
/*******************************************/
/**          W E L C O M E                **/
/**               T O                     **/
/**  piHPSDR Controller for Radioberry    **/
/**               V1.0                    **/
/**-------------------------------------- **/
/**          DE VU2DLE Dileep             **/
/*******************************************/
```
Enjoy and Happy Homebrewing….

Many 73s
DE VU2DLE Dileep
