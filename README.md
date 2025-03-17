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
On Bareboat Necessities (BBN) OS (full)

````
# shutdown signalk
sudo systemctl stop signalk

if [ -f bbn-flash-alarm-A.sh ]; then rm bbn-flash-alarm-A.sh; fi
wget https://raw.githubusercontent.com/bareboat-necessities/my-bareboat/refs/heads/master/m5stack-tools/bbn-flash-alarm-A.sh
chmod +x bbn-flash-alarm-A.sh 
./bbn-flash-alarm-A.sh -p /dev/ttyACM1

````
