# bbn_alarms_A
Boat alarms and monitoring on esp32

## Hardware

- atomS3-lite by m5stack:  https://shop.m5stack.com/products/atoms3-lite-esp32s3-dev-kit
- PoESP32 Ethernet unit by m5stack:  https://shop.m5stack.com/products/esp32-ethernet-unit-with-poe
- Voltmeter Unit (ADS1115) by m5stack:  https://shop.m5stack.com/products/voltmeter-unit-ads1115
- Atomic PortABC Extension Base by m5stack:  https://shop.m5stack.com/products/atomic-portabc-extension-base
- JSN-SR04T Waterproof Ultrasonic Sensor:  https://www.makerguides.com/interfacing-esp32-and-jsn-sr04t-waterproof-ultrasonic-sensor/

## Software

### Default behavior

By default without any configuration changes it acts as NMEA XDR sensor sending NMEA 0183 sentences via USB serial port at 115200 baud rate.


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

### Setting alarm thresholds

Voltage threshold (Volts):

````
curl "http://192.168.1.53/cfg?volt=11.7"
````


Bilge level threshold (cm):

````
curl "http://192.168.1.53/cfg?bilge=25.0"
````

