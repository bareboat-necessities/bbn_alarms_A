# bbn_alarms_A
Boat alarms and monitoring on esp32.

Open source device for sending high bilge level and low battery voltage alarms via WhatsApp.
It also can be used as a simple bilge level and battery voltage NMEA-0183 XDR sensor.

No soldering required. Connect sensors to i2c. Load firmware to atomS3-lite. (On Bareboat Necessites OS it's just copy-paste of a script, see below). Plug and play.

## Hardware

- atomS3-lite by m5stack:  https://shop.m5stack.com/products/atoms3-lite-esp32s3-dev-kit
- PoESP32 Ethernet unit by m5stack:  https://shop.m5stack.com/products/esp32-ethernet-unit-with-poe
- Voltmeter Unit (ADS1115) (for battery voltage) by m5stack:  https://shop.m5stack.com/products/voltmeter-unit-ads1115
- Atomic PortABC Extension Base by m5stack:  https://shop.m5stack.com/products/atomic-portabc-extension-base
- JSN-SR04T Waterproof Ultrasonic Sensor (for bilge water level):  https://www.makerguides.com/interfacing-esp32-and-jsn-sr04t-waterproof-ultrasonic-sensor/

## Software

### Default behavior

By default without any configuration changes it acts as NMEA XDR sensor sending NMEA 0183 sentences (for bilge level and battery voltage) via USB serial port at 115200 baud rate.

````
$BBTXT,01,01,01,FirmwareTag: bbn_alarms_A*0F
$BBTXT,01,01,01,Loaded settings. phone:17321111111 apiKey:1111111*15
$BBTXT,01,01,01,Loaded config. voltageThreshold:11.70*7C
$BBTXT,01,01,01,Loaded config. bilgeThreshold:25.00*7B
$BBTXT,01,01,01,VOLTAGE found ads1115 sensor at address=0x49 alt bus*2F
$BBTXT,01,01,01,Waiting for ethernet device connected*1F
$BBTXT,01,01,01,Resetting device*20
$BBTXT,01,01,01,Waiting for ethernet device connected*1F
$BBXDR,U,0.000,V,VOLT_1*0C
$BBXDR,D,0.20,M,Range_JSN_S04T*50
$BBTXT,01,01,01,Waiting for ethernet connected*27
$BBTXT,01,01,01,Waiting for ethernet connected*27
$BBTXT,01,01,01,LocalIP: 192.168.1.53*0B
$BBTXT,01,01,01,Got time from NTP*6C
$BBXDR,U,0.000,V,VOLT_1*0C
$BBXDR,D,0.20,M,Range_JSN_S04T*50
$BBTXT,01,01,01,Waiting for web server start*7C
$BBXDR,U,0.000,V,VOLT_1*0C
$BBXDR,D,0.20,M,Range_JSN_S04T*50
$BBTXT,01,01,01,Received data from connID: 0*7B
$BBTXT,01,01,01,Going to sleep mode*46

````

### Integration with SignalK

Integration with SignalK is done via NMEA XDR Parser SignalK plugin.

More: https://github.com/GaryWSmith/xdr-parser-plugin

Recommended config file for XDR Parser SignalK plugin:

https://github.com/bareboat-necessities/lysmarine_gen/blob/bookworm/install-scripts/4-server/files/xdrParser-plugin.json

## Loading Firmware

### On Bareboat Necessities (BBN) OS (full)

````
# shutdown signalk
sudo systemctl stop signalk

if [ -f bbn-flash-alarms-A.sh ]; then rm bbn-flash-alarms-A.sh; fi
wget https://raw.githubusercontent.com/bareboat-necessities/my-bareboat/refs/heads/master/m5stack-tools/bbn-flash-alarms-A.sh
chmod +x bbn-flash-alarms-A.sh 
./bbn-flash-alarms-A.sh -p /dev/ttyACM1

````

### Reading serial port

````
stty -F /dev/ttyACM1 38400
socat stdio /dev/ttyACM1
````

## Configuration

To find IP address of the device check output from serial port.

### Setting WhatsApp API key and phone:

````
curl "http://192.168.1.53/settings?phone=17321111111&key=1111111"
````

Power cycle the device and after that it should run in alarms mode (15 min cat naps/sleeps with 2 minutes run cycles).

### Setting alarm thresholds

Voltage threshold (Volts):

````
curl "http://192.168.1.53/cfg?volt=11.7"
````


Bilge level threshold (cm):

````
curl "http://192.168.1.53/cfg?bilge=25.0"
````

