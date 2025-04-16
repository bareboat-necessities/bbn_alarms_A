# bbn_alarms_A
Boat alarms and monitoring on esp32.

Open source device for sending high bilge level and low battery voltage alarms via WhatsApp.
It also can be used as a simple bilge level and battery voltage NMEA-0183 XDR sensor.

<p align="center">
<img src="./img/bbn_alarms1.jpg?raw=true" style="width: 40%; height: auto;" alt="BBN Alarms pic1" />
<img src="./img/bbn_alarms5.jpg?raw=true" style="width: 40%; height: auto;" alt="BBN Alarms pic1" />
</p>

No soldering required. Connect sensors to Atomic PortABC Extension Base. Load firmware to atomS3-lite. (On Bareboat Necessites OS it's just copy-paste of a script, see below). Plug and play. IKEA furniture will take you longer to put thogether than this assuming you got the parts.

For sensing alarms the device needs to be connected to the Ethernet port of the boat router via Ethernet cable.

<p align="center">
<img src="./img/bbn_alarms_6.jpg?raw=true" style="width: 40%; height: auto;" alt="BBN Alarms pic1" />
<img src="./img/bbn_alarms_1.jpg?raw=true" style="width: 40%; height: auto;" alt="BBN Alarms pic2" />
</p>


## Hardware

- atomS3-lite by m5stack:  https://shop.m5stack.com/products/atoms3-lite-esp32s3-dev-kit
- PoESP32 Ethernet unit by m5stack:  https://shop.m5stack.com/products/esp32-ethernet-unit-with-poe
- Voltmeter Unit (ADS1115) (for battery voltage) by m5stack:  https://shop.m5stack.com/products/voltmeter-unit-ads1115
- Atomic PortABC Extension Base by m5stack:  https://shop.m5stack.com/products/atomic-portabc-extension-base
- JSN-SR04T Waterproof Ultrasonic Sensor (for bilge water level):  https://www.makerguides.com/interfacing-esp32-and-jsn-sr04t-waterproof-ultrasonic-sensor/

### Accessories

- 4.7"x3.5"x2.7" waterproof box with baseplate, clear hinged cover, mounting legs, stainless steel hardware
- waterproof cable glands
- grove cables
- standoffs
- stainless steel mounting screws
- m5stack screw kit:   https://shop.m5stack.com/products/16-pcs-m3-12-18-25-32-screw-with-allen-key

### JSN-SR04T Waterproof Ultrasonic Sensor to NMEA 0183 XDR

https://www.makerguides.com/interfacing-esp32-and-jsn-sr04t-waterproof-ultrasonic-sensor/

https://bitbucket.org/teckel12/arduino-new-ping/wiki/Home

Connecting to m5stack atomS3-Lite

Pins:

- G8 to Echo_TX
- G7 to Trig_RX
- VCC 3.3 to VCC
- GND to GND

Bilge Level Sensor

Install it into a cap of PVC pipe.
Drill a hole for air to escape.
Mount PVC pipe with sensor on top of it
in a bilge. The sensor will measure distance to water surface in the bilge,
giving you bilge level.

<p align="center">
<img src="./img/bilge_level_sensor.jpg?raw=true" style="width: 40%; height: auto;" alt="BBN Bilge Level Sensor" />
</p>


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

Long press side button on atomS3 till you see green to enter the mode for uploading firmware.

NOTE: /dev/ttyACM1 in the script below is for example. In your case device name might be different. You can find out what it is by
inspecting differences in output of 

```
ls -ltr /dev/tty*
```

with the device unplugged from USB and plugged into USB.


#### Load the firmware:

````
# shutdown signalk
sudo systemctl stop signalk

if [ -f bbn-flash-alarms-A.sh ]; then rm bbn-flash-alarms-A.sh; fi
wget https://raw.githubusercontent.com/bareboat-necessities/my-bareboat/refs/heads/master/m5stack-tools/bbn-flash-alarms-A.sh
chmod +x bbn-flash-alarms-A.sh 
./bbn-flash-alarms-A.sh -p /dev/ttyACM1

````

Unplug and plug the device into USB to reboot.

### Reading serial port

````
stty -F /dev/ttyACM1 115200
socat stdio /dev/ttyACM1
````

## Configuration

To find IP address of the device check output from serial port.

Instructions to generate API key to send WhatsApp messages: https://www.callmebot.com/

### Setting WhatsApp API key and phone:

Connect your device to the Ethernet port on your boat router, power on via USB. Open http://192.168.1.53/ in the browser, fill out the required fields on the form and click 'Submit'.

NOTE: You device IP address might be different, so check the output from USB serial port.

<p align="center">
<img src="./img/bbn_alarms_api_key.png?raw=true" style="width: 60%; height: auto;" alt="BBN Bilge Level Sensor" />
</p>

Or you could do it via command line:

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

## Other Bareboat Necessities Devices

Project Home:  https://bareboat-necessities.github.io/

- Sensors Hub: https://github.com/bareboat-necessities/bbn_sensors_hub_AB
- Engine Sensors Box: https://github.com/bareboat-necessities/bbn_sensors_hub_C
- NMEA N2K to USB: https://github.com/bareboat-necessities/bbn-m5-s3-n2k-usb
- Instruments Displays on esp32: https://github.com/bareboat-necessities/bbn-m5stack-tough
- Boat Heave Sensor: https://github.com/bareboat-necessities/bbn-wave-period-esp32
- I2C over USB for Linux: https://github.com/bareboat-necessities/bbn-i2c-over-usb
- N2K Senders: https://github.com/bareboat-necessities/bbn-m5-s3-n2k-i2c
