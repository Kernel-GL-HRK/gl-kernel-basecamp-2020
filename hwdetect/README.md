# Home work hot plugged hardware detector

Create the script 'hwdetect.sh' which detects connected hardware.
For monitor hardware from user level please use /dev folder,
output from lsusb and i2detect (optional).
Most interesting devices are:
 - USB to TTL convertors,
 - flash drives,
 - SD cards,
 - i2c devices.

# Solution

Script provides access to the current list of equipment at the start of running.
The further step is a loop handle of device connect/disconnect actions.
User shortcut interrupting calls cleanup function before exit.

Platform run difference:
 - desktop doesn't show the first flash device (often - main storage device),
 - armv7l (OrangePi One) also print strings of i2cdetect output with available devices. This option need root.
 
 Examples:
  - run on desktop
  
  Console output:
  
Device directory: /dev
1 - TTY USB:
ttyUSB0
2 - Flash devices:
sdb
3 - SD cards:
none

Plug/unplug actions:
Press Ctr-C to exit...
-sdb
+mmcblk0
-ttyUSB0
-mmcblk0
+sdb
^C
Clean exit

  Description:
  At the start in this system there were TTY USB and flash device. 
  sdb was removed and added again,
  mmcblk0 (SD Card) was added and after removed,
  TTY USB was removed.

  - run on OrangePi One
  
  Console output:

...
2 - Flash devices:
none
3 - SD cards:
mmcblk0
4 - i2c adresses:
30: -- -- -- -- -- -- -- -- -- -- -- -- 3c -- -- -- 
...

  Description:
  Current devices are SD Card (can be removed) and i2c oled which has address 0x3c.
