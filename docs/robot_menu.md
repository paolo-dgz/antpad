# Guide to navigate v0.1.2 settings menu directly from the remote

to enter the menu turn on the robot without turining on the controller, once the builtin led will start blinking faster turn on the controller 

when the controller is connected the led will start blinking slowly a number of time corresponding to the current setting

if the board is connected to a serial monitor it will output more detailed settings and navigation feedback

```
robot on 6 seconds before the remote
└── navigation
    ├── B exit menu
    ├── A enter current settings
    ├── 1) servo settings
    │   └── B restart without saving
    │   └── A Save and go back
    ├── 2) motors settings
    │   └── B restart without saving
    │   └── A Save and go back
    ├── 3) remote settings
    │   └── B restart without saving
    │   └── A Save and go back
    ├── 4) board settings
    │   ├── B restart without saving
    │   └── A save and restart
    ├── 5) blank, will be used in the future
    └── 6) factory reset
        ├── B restart
        └── A factory reset and restart
```

## navigation commands: 
in navigation menu the board will failsafe and no controller input will be relied to the board, once a setting is selected the led will blink faster a number of time corresponding to the current setting
- up: move to next setting
- down: move to previous setting
- A: select setting
- B: leave menu

### 1) servo commands: 
while in servo mode the current editing EPA for the current editing servo will be sent in place of the controller input, swap to the other servo to allow controller input and check inveret/normal direction 
all motor inputs will be live when in servo setting

- up: edit servo 0/A/dc
- down: edit servo 1/B/dc
- left: lower current endpoint
- right: increase current endpoint
- Y: invert/normalize current servo
- X: change editing endpoint

- B: restart robot (does not save)
- A: save and go back to **navigation**

### 2) motors commands:
- up: invert/normalize motor C/W
- left: invert/normalize motor L
- right: invert/normalize motor R
- Y: swap motor R and L values

- B: restart robot (does not save)
- A: save and go back to **navigation**

### 3) remote commands:

- right: invert/normalize ch 1 (steering)
- down: invert/normalize ch 2 (throttle)
- up: invert/normalize ch 3 (servo 0/A/dc)
- left: invert/normalize ch 4 (servo 1/b, motor W)
- Y: invert/normalize stick mode.  
    - **normal** (default) ch 1 and 2 are on right stick, ch3 and 4 on left stick  
        - ch5 positive values are on LB/L1
    - **inverted** ch 3 and 4 are on right stick, ch1 and 2 on left stick
        - ch5 positive values are on RB/R1  
- X: center/offset ch3:
    - **offset** (default) ch3 stick will only work from center -100% to top or bottom edge +100% based on reverse
    - **centerd** ch3 stick will work from bottom edge -100% to top edge +100%  

- B: restart robot (does not save)
- A: save and go back to **navigation**

### 4) board
- up: enable dc servo mode
    - **when on dc servo mode a pot must be connected to servo b pin or dedicated pot pin**
    - servo a pin will output servo b channel
    - servo b pin will no longer output pwm
    - motor w will move according to pot position, controlled by servo a channel    
- down: disable dc servo mode (defualt)
    - servo a pin will output servo a channel
    - servo b pin will output servo b channel
    - motor w pin will output motor w channel input
- right: enable servo stretcher
    - now servo outputs will send 500-25000us pulse
- left: dissable servo stretcher (default)
    - now servo outputs will send 1000-2000us pulse

- B: restart robot (does not save)
- A: save and restart robot

### 5) to be implemented

- B: go back to **navigation**
- A: go back to **navigation**

### 6) factory reset

- B: go to **navigation** (does not restart)
- A: clean EEPROM and restart robot
