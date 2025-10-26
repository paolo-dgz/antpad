This is a guide to stop one of the wheel motors from moving when using esp32c3supermini together with I Robottari ASD shield.
The glitch is caused by serial debug info getting sent on motor pads due to the FUSE settings on c3 boards, clearing this fuses fixes the issue but prevents from using an external debugger attahced to such pins, usb debug will remain untouched.

install python https://www.python.org/downloads/

open a terminal and run 

```
pip install esptool
```

connect your esp to the pc via USB

use and arduino ide or device manager to get your COM port, copy and overwrite COM4 in the following lines


run the following command to check the current state of the fuse (should be false)

```
espefuse.py --port COM4 summary | FINDSTR DIS\_USB\_SERIAL\_

```

run the following command to disable the PAD debug function

```
espefuse.py --port COM4 burn\_efuse DIS\_PAD\_JTAG 1
```

note: the command will ask to type BURN to confirm the operation, burning fuses is an irreversible action