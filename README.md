# Antpad
A fimrware to use bluepad32 for antweight robots with esp32c3 microcontrollers, with failsafe and binding checks in place for added safety on top of tank steering mixing, verios end point and channel reverse configurations. Designed to be ready to co with "ASD i Robottari" esp32c3 shield.

Notice:
Combat robots are inherently dangerous, always use one with the correct weapon locks and in an enclosed testbox, no robot or firmware is 100% safe, especilly through bluetooth connection, it's advised to use this code only for non dangerous sportsman robots, any use with destructive robots such as spinner is considered a misuse of the software and there is no guarateed that the software will prevent all misbehaviour.

to uplaod the code install the bluepad32 boards on your arduino IDE, then navigate to source/antpad_arduino/ and open the antpad_arduino.ino file, remember to select board->esp32_bluepad32->"esp32c3 dev module" and enable usb CDC on boot in the tools tab on arduino IDE

optionally run the espefuse commands to prevent the esp to move motors while on upload or reset state

# Bluepad32

This esp32c3 code is based on the awesome [bluepad32 software][bluepad32-github], please visit them to setup your arduino ide in order to upload the code 

# Support

## antpad support
you can ask about antpad in the italian robot combat [Discord][discord]


## bluepad32 support
* [Documentation][docs] [![Documentation Status](https://readthedocs.org/projects/bluepad32/badge/?version=latest)](https://bluepad32.readthedocs.io/?badge=latest)
* [Discord][discord_bp32] [![discord](https://img.shields.io/discord/775177861665521725.svg)](https://discord.gg/r5aMn6Cw5q)

[docs]: https://bluepad32.readthedocs.io/

[discord_bp32]: https://discord.gg/r5aMn6Cw5q
[discord]: https://discord.gg/zKuerqs6EP


## License

Antpad is open source, [licensed under Apache 2][apache2].

However, Bluepad32 and thus Antpad depends on the great [BTstack library][btstack-github]. Which is free to use for
open source projects. But commercial for closed-source projects.

If you are developing a commercial product for:

- ESP32: [You should contact BTstack people][btstack-homepage].
- Pico W: [You are already covered by Raspberry Pi License][rpi-btstack-license].

Notice: I’m not affiliated with BTstack or bluepad32 people.

[btstack-github]: https://github.com/bluekitchen/btstack

[apache2]: https://www.apache.org/licenses/LICENSE-2.0

[btstack-homepage]: https://bluekitchen-gmbh.com/

[rpi-btstack-license]: https://github.com/raspberrypi/pico-sdk/blob/master/src/rp2_common/pico_btstack/LICENSE.RP

[bluepad32-github]: https://github.com/ricardoquesada/bluepad32