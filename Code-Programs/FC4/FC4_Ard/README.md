#Avionics #FC #cpp


# Flight Computer (SD1000)


The full fledged C lines that governs the flight and end result of the historic SD1000 launch vehicle.


# Features

1. Modular Finite State Machine logic for better memory efficiency and modularity.This enables easy debugging,scalability and improves readability.

2. The FSM follows the following states

	 >IDLE
	 >INIT
	 >DEBUGGING
	 >LAUNCH
	 >TRIG1 (Parachute)
	 >TrRIG2 (Payload)
	 
3. The following built in libraires are being used

   + Wire (I2C communication)
   + SPI (SPI communication)
   + MPU6050 ( IMU sensor)
   + Adafruit_Sensor
   + Adafruit_BMP085 (Pressure module)
   + LoRa (By Sandeep Mistry (LoRa Telemetry))
   + SD (Data logging & MicroSD)


3. Timed events . Every event is tracked from the starting, Triggers of Payload , Parachute and even Ignition.


4. System health is monitored continuosly every time till LAUNCH. afterwards logic shifts to sampling and sending . 

5. Modularity. The entire algorithm is split into various C++ files which govern individual functionality. Any change in any sensor reading can be modified without cracking the whole program.


## Directory Structure

```markdown
Project/
├── config.h          // Pin definitions, constants, and configurations
│
├── imu.h             // IMU (MPU6050) sensor header
├── imu.cpp           // IMU implementation
│
├── bmp180.h          // BMP180 barometer sensor header
├── bmp180.cpp        // BMP180 implementation
│
├── lora.h            // LoRa communication header
├── lora.cpp          // LoRa implementation
│
├── sdcard.h          // SD card logging header
├── sdcard.cpp        // SD card implementation
│
├── mode.h            // Flight mode manager (INIT, LAUNCH, etc.)
├── mode.cpp          // Mode logic implementation
│
├── utils.h           // Utility functions header
├── utils.cpp         // Utility functions implementation
│
├── main.ino          // Main Arduino sketch (entry point, loop, setup)


```



## Working


### 1. Declarations

 following enums were declared representing the states and health respectively.
 

```cpp
enum FlightMode { IDLE, INIT, ARMING, DEBUGGING, LAUNCH, TRIG1, TRIG2 };
enum SysStatus { OK, HALT };

```
 
 The main file has
 
```cpp
	FlightMode currentMode = IDLE;
	SysStatus systemStatus = OK;
```

These are default states which are updates consequently.

### 2.Setup()

1.  We proceed if and only if all the modules get initialised.
 
```cpp
if(initIMU() && initBMP() && initLoRa() && initSD()){
    Serial.println("sensors ok!!!");
    systemStatus = OK;
    calibrateIMU();
```

All the init functions are defined in their modules.
calibrateIMU() is run and the IMU offsets were set.


2. We open a datafile and prints status.

```cpp
	datafile = SD.open("samples.csv",FILE_WRITE);
	datafile.println(F("T(ms),Ax,Ay,Az,Gx,Gy,Gz,Alti,Pre,Temp"));
```

These are the samplings which will be recorded.

**System health is checked and these steps are run only if sensors are initialised.**


### 3. loop()

	The LoRa command is checked continuosly for input


```cpp
String cmd = checkCmd();  // Get command from LoRa
```

Here

```text
If cmd = ARM
	+ Switch to INIT state
		+ Run initialise() && switch to ARM state
		+ Wait in ARM state , till LAUNCH command is recieved.
+ if cmd = LAUNCH
	+ Switch to LAUNCH state
		+ logs Data
		+ Wait for payload and parachute Triggers.
+ if cmd = ABORT
	+ Switch to IDLE state.
		+ Model can be inspected as no risk of accidental launch
```




**The Change of states are governed by the modemanager() function. The working is as below**


### 4. modemanager()

A bunch of switch statements govern the flow

```cpp

switch (currentMode) {
    case IDLE:
      break;  

    case INIT:
      initialize();
      break;

    case ARMING:
      countdown();
      break;  

    case DEBUGGING:
      debugging();
      break;
  

    case LAUNCH:
      launch();
      break;
 

    case TRIG1:
      deploypayload();
      break;

 
    case TRIG2:
      deployparachute();
      break;
  }
```


*** Here TRIG1 and TRIG2 have been removed as it made not much difference***

** The flow stays in LAUNCH even during PAYLOAD and PARACHUTE triggers**



**The modemanager() function was called twice in the loop()**
**At first it changes state from INIT to ARMING only after initialising sensors
Second it stays in LAUNCH for the rest of the journey**



### State Diagram


```markdown
stateDiagram-v2
    [*] --> IDLE

    IDLE --> INIT: ARM command
    IDLE --> LAUNCH: LAUNCH command
    IDLE --> [*]: ABORT

    INIT --> LAUNCH: LAUNCH command
    INIT --> IDLE: ABORT

    LAUNCH --> DEBUGGING: System Error
    LAUNCH --> IDLE: ABORT

    DEBUGGING --> IDLE: Reset/Abort

    LAUNCH --> LAUNCH: 
        - Log data every 1s  
        - Send telemetry every 2s  
        - Check PAYTIME & PARATIME triggers



```








