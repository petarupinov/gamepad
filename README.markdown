AT90USBKEY GAMEPAD
==================

This port using board atmel boards [AT90USBKEY][1]/[AT90USBKEY2][1]
based on microcontrollers [AT90USB1287][2], [schematic and hardware guide][3]

Before flashing the source is needed follow these steps:

1. Erase microcontroller via JTAG programmer [AVRDragon][4] or [AVR JTAGICE mkII][5] - this will erase bootloader and unlock to reconfigure fuse bits
2. Reflash bootloader "bl_usb.a90" from the flash memory mass storage device path: "/libraries/bootloader/AVR USB/bl_usb.a90"
3. Reconfigure fuse bits like as: Extened 0xF3, High 0x99, Low 0xDE
4. Compile the project on [AVR Studio 4 (build 4.17)][6] or Programmers Notepad using avr-gcc ([WinAVR 20100110][7]) 4.3.3
5. Flash the hex file via [ATmelFlip][8] for Windows or [dfu-programmer][9] for Linux or MacOSX


There are datas in flash memory and you can show the data when flash this example[AVR273: USB Mass Storage Implementation on megaAVR with USB][10]
Also you can try all other usb examples


[1]: http://www.atmel.com/tools/at90usbkey.aspx
[2]: http://www.atmel.com/images/doc7593.pdf
[3]: http://www.atmel.com/Images/doc7627.pdf
[4]: http://www.atmel.com/tools/avrdragon.aspx
[5]: http://www.atmel.com/tools/avrjtagicemkii.aspx
[6]: http://www.atmel.com/dyn/resources/prod_documents/AvrStudio417Setup.exe
[7]: https://sourceforge.net/projects/winavr/files/WinAVR/20100110/WinAVR-20100110-install.exe/download
[8]: http://www.atmel.com/tools/flip.aspx
[9]: http://dfu-programmer.github.io/
[10]: http://www.atmel.com/tools/at90usbkey.aspx?tab=documents
