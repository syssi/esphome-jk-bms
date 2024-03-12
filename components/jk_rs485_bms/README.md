# esphome JK-PB BMSs

![GitHub stars](https://img.shields.io/github/stars/txubelaxu/esphome-jk-bms)
![GitHub forks](https://img.shields.io/github/forks/txubelaxu/esphome-jk-bms)
![GitHub watchers](https://img.shields.io/github/watchers/txubelaxu/esphome-jk-bms)

ESPHome component to monitor a Jikong Battery Management System (JK-PB) via ESP SERIAL (UART-TTL) + SERIAL TO RS485 + ETHERNET CABLE
In theory 1 ESP can gather information of every BMS in the RS485 network: MAX 16
The bluetooth is free to use with your mobile.

## Supported devices

JK-PBx models with software version `>=14.0` are using the implemented protocol and should be supported.

* JK-PB2A16S-20P, hw 14.XA, sw 14.20, using `JK02_32S` (reported by [@txubelaxu]

## Untested devices

* JK-PB2A16S-20P, hw 15.XA,

## Requirements

* [ESPHome 2022.11.0 or higher](https://github.com/esphome/esphome/releases).
* Generic ESP32 or ESP8266 board
* SERIAL TTL to RS485 converter
* Ethernet type cable (one side with RJ45, the other side 3 wires: A, B and GND)

## Schematics
![image](https://github.com/txubelaxu/esphome-jk-bms/assets/156140720/bc6fa31c-2421-4f10-9604-e111d3943636)
<BR>
(<a href="https://www.google.com/url?sa=i&url=https%3A%2F%2Fforum.arduino.cc%2Ft%2Fcommunication-softwareserial-using-rs485-module-with-esp32%2F932789&psig=AOvVaw1k5Ukv6hEwIAS9I4HR-IlD&ust=1710240770923000&source=images&cd=vfe&opi=89978449&ved=0CBUQjhxqFwoTCMj-uNWF7IQDFQAAAAAdAAAAABAD">image source</a>)



```
                              CONVERTER                    UART-TTL
┌──────────┐                ┌───────────┐                ┌─────────┐
│          │<----- A  ----->│  SERIAL   │------ RX ----->│         │
│  JK-BMS  │<----- B  ----->│  TTL TO   │                │ ESP32/  │
│          │<----- GND ---->│  RS485    │<----- GND ---->│ ESP8266 │<-- Vcc
│          │                │ CONVERTER │                │         │<-- GND
└──────────┘                └───────────┘                └─────────┘

# ESP32 UART-TTL               
┌─── ─────── ────┐
│                │
│ O   O   O   O  │
│GND  RX  TX VBAT│
└────────────────┘
  │   │   
  │   │   
  │   └─────── GPIO16
  └─────────── GND

# HOW TO CONNECT RS485-2 BUS:
FIRST: CONNECT ALL THE BMSs IN CHAIN
SECOND: CONNECT THE SNIFFER (ESP32->RS485 CONVERTER->) AT ONE END OF THE CHAIN
```
![image](https://github.com/txubelaxu/esphome-jk-bms/assets/156140720/ac435a4e-5b1d-4e07-b820-296351084d9c)

![image](https://github.com/txubelaxu/esphome-jk-bms/assets/156140720/9505df34-a807-4953-b0b9-946aa2de66e6)
pinout:
![image](https://github.com/txubelaxu/esphome-jk-bms/assets/156140720/278abcac-c095-4ae2-8ba2-f82f1f152343)

```

Usable any of the two connectors:
┌─────────────────────────┐       ┌─────────────────────────┐
│                         │       │                         │
│ O  O  O  O  O  O  O  O  │       │ O  O  O  O  O  O  O  O  │
│ 1  2  3  4  5  6  7  8  │       │ 9  10 11 12 13 14 15 16 │
└─────────────────────────┘       └─────────────────────────┘
  │  │  │                           │  │  │   
  │  │  └──── GND                   │  │  └──── GND 
  │  └─────── A                     │  └─────── A
  └────────── B                     └────────── B


```

## Installation

just use the `esp32-example-jkpb-rs485.yaml` as proof of concept:

```bash
# Install esphome
python3 -m pip install esphome

# Clone this external component
git clone https://github.com/syssi/esphome-jk-bms.git
cd esphome-jk-bms

# Create a secrets.yaml containing some setup specific secrets
cat > secrets.yaml <<EOF
wifi_ssid: MY_WIFI_SSID
wifi_password: MY_WIFI_PASSWORD

mqtt_host: MY_MQTT_HOST
mqtt_username: MY_MQTT_USERNAME
mqtt_password: MY_MQTT_PASSWORD
EOF

# Configure all the BMS in this yaml, assigning a name and the address of each one
- IMPORTANT THINGS:
  * 1 (ONLY ONE) BMS MUST BE MASTER IN THE RS485 NETWORK --> 0x00 (addressed) Use DIP switches to assign this address.
  * OTHER BMS ARE SLAVE IN THE RS485 NETWORK. What address for each slave? Anyone different to 0x00. Each slave one different address, of course. Use DIP switches to assign the selected address.

# Validate the configuration, create a binary, upload it, and start logs
esphome run esp32-example-jkpb-rs485.yaml

## Known issues

* The sensor configuration is in progress.

## Goodies

A user of this project ([@dr3amr](https://github.com/dr3amr)) shared some [Home Assistant Lovelace UI cards for a beautiful dashboard here](https://github.com/syssi/esphome-jk-bms/discussions/230).

![Custom Lovelace UI cards](images/lovelace-cards-contribution.jpg "Home Assistant Lovelace UI cards")

## Debugging

If this component doesn't work out of the box for your device please update your configuration to enable the debug output of the UART component and increase the log level to the see outgoing and incoming serial traffic:

```
logger:
  level: DEBUG

```

## References

* https://secondlifestorage.com/index.php?threads/jk-b1a24s-jk-b2a24s-active-balancer.9591/
* https://github.com/jblance/jkbms
* https://github.com/jblance/mpp-solar/issues/112
* https://github.com/jblance/mpp-solar/blob/master/mppsolar/protocols/jk232.py
* https://github.com/jblance/mpp-solar/blob/master/mppsolar/protocols/jk485.py
* https://github.com/sshoecraft/jktool
* https://github.com/Louisvdw/dbus-serialbattery/blob/master/etc/dbus-serialbattery/jkbms.py
* https://blog.ja-ke.tech/2020/02/07/ltt-power-bms-chinese-protocol.html
