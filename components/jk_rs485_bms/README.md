# esphome gathering info from multiple JK-PB BMSs usign RS485 internal network

![GitHub stars](https://img.shields.io/github/stars/txubelaxu/esphome-jk-bms)
![GitHub forks](https://img.shields.io/github/forks/txubelaxu/esphome-jk-bms)
![GitHub watchers](https://img.shields.io/github/watchers/txubelaxu/esphome-jk-bms)

ESPHome component to monitor a Jikong Battery Management System (JK-PB) via ESP SERIAL (UART-TTL) + SERIAL TO RS485 + ETHERNET CABLE
In theory 1 ESP can gather information of every BMS in the RS485 network: MAX 16
The BMS bluetooth remains free to use with your mobile.

## REMINDER

* Use this at your own risk

## Supported devices

JK-PBx models with software version `>=14.0` are using the implemented protocol and should be supported.

* JK-PB2A16S-20P, hw 14.XA, sw 14.20, using `JK02_32S` (reported by [@txubelaxu])
* JK-PBx, hw 15.XA, sw 15.11, using `JK02_32S` (reported by [@denveronly])

## Untested devices

* JK-PB1A16S-20P, hw 15.XA,

## Requirements

* [ESPHome 2022.11.0 or higher] (https://github.com/esphome/esphome/releases).
* Generic ESP32 or ESP8266 board
* SERIAL TTL to RS485 converter
* Ethernet type cable (one side with RJ45, the other side 3 wires: A, B and GND)


# IMPORTANT THINGS ABOUT RS485:
  * IN THEORY, IN A RS485 NETWORK ONE (ONLY ONE) DEVICE CAN BE THE MASTER.
  * In a JK-RS485 internal netwok:
     + the MASTER is the one that has 0x00 (address) Use DIP switches to assign this address.
     + other BMSs connected to the JK-RS485 internal network need a different address. So: 0x01 or 0x02 or 0x03... or 0x0F
     + each BMS in the network must have a different address. Use DIP switches to assign these addresses.

The ESP with this code needs NO RS485 address to work.. But it needs a yaml file configured properly. The yaml content must be equal to the real situation of the amount of BMSs and their addresses in the network. How does ESP code work?

MODE 1. If there is a JK-BMS acting as REAL MASTER (i.e. it has set 0x00 address), the code will be sniffing all the traffic in the network. The problem is that there is some information that is NEVER in the sniffed traffic information (device info frame type 03): device-name, hw version, sw version, password, RCV Time, RFV Time...
So, periodically, ESP code will act as a PSEUDO-MASTER to try to get this information. It works for every slave device, but not for master device. At this moment, master device does NOT broadcast by itself the "frame type 03" info. And it does not answer to a "frame type 03" request sent by PSEUDO MASTER.

MODE 2. If every JK-BMS are set as SLAVES (all of them), the ESP code will act as MASTER. So, if ESP code does not detect any REAL MASTER in the network, it will act as a MASTER. Remember to adapt DIP switches and the YAML (addresses) as well. Changing the addreses of the devices does not affect to the historic data gathered from that device, because the config is based on the NAME of the device and not on the address of the device. So, change the address, but not the name. In this mode, ESP will gather all the information (cell info, device settings and device info). If anytime, a REAL MASTER arrives to the network, ESP will detect it and will stop acting as MASTER.

In any of the two modes ESP needs to SPEAK to the JK-RS485 internal network. That is why it needs a "SPEAKING PIN OUT" to direct a signal to the "TTL to RS485" converter. There are schematics below. It is not mandatory to connect this pin to gather some info. But if it is not connected it could not ask for some information, and it can not act as a MASTER. But the code should work in the same way until this version. Any way, connected or not, it must be a "talk pin" in the config.




## Schematics

```
                              CONVERTER                          UART-TTL
┌──────────┐                ┌───────────┐                       ┌─────────┐
│          │<----- A  ----->│  SERIAL   │<-----------------Vcc--│         │<--Vcc
│  JK-BMS  │<----- B  ----->│  TTL TO   │                       │ ESP32/  │
│          │                │  RS485    │--DI---------------RX->│ ESP8266 │
│          │                │           │<-RO---------------TX--│         │
│          │                │ CONVERTER │<-DE-----+             │         │
│          │                │           │<-RE-----└---TALK PIN--│         │
│          │                │           │                       │         │
|          |<------GND----->|           |<----------GND-------->|         |<--GND
└──────────┘                └───────────┘                       └─────────┘      
                                  

# ESP32 UART-TTL (PINOUT ASIGNMENT EXAMPLE)
┌──────────────────────────────┐                
│                              │
│ O     O     O     O     O    │
│ GND   RX    TX    talk  Vcc  │
└──────────────────────────────┘
  ^     ^     ^     ^     ^   
  │     │     │     |     └──── Vcc
  │     │     |     └────────── GPIO16
  │     │     └──────────────── GPIO17
  │     └────────────────────── GPIO4
  └──────────────────────────── GND

# HOW TO CONNECT RS485-2 BUS:
FIRST: CONNECT ALL THE BMSs IN CHAIN
SECOND: CONNECT THE SNIFFER (ESP32->RS485 CONVERTER->) AT ONE END OF THE CHAIN
```
![image](https://github.com/txubelaxu/esphome-jk-bms/assets/156140720/b9ef1522-cd68-4ab7-a709-1c7efb24b0ca)

![image](https://github.com/txubelaxu/esphome-jk-bms/assets/156140720/9505df34-a807-4953-b0b9-946aa2de66e6)
pinout:
![image](https://github.com/txubelaxu/esphome-jk-bms/assets/156140720/278abcac-c095-4ae2-8ba2-f82f1f152343)

```
Usable any of the two connectors:
┌─────────────────────────┐           ┌─────────────────────────┐           ┌─────────────────────────┐           ┌─────────────────────────┐
│                         │           │                         │           │                         │           │                         │
│ O  O  O  O  O  O  O  O  │    OR     │ O  O  O  O  O  O  O  O  │    OR     │ O  O  O  O  O  O  O  O  │    OR     │ O  O  O  O  O  O  O  O  │
│ 1  2  3  4  5  6  7  8  │           │ 9  10 11 12 13 14 15 16 │           │ 1  2  3  4  5  6  7  8  │           │ 9  10 11 12 13 14 15 16 │
└─────────────────────────┘           └─────────────────────────┘           └─────────────────────────┘           └─────────────────────────┘
  │  │  │                               │  │  │                                              │  │  │                               │  │  │   
  │  │  └──── GND                       │  │  └──── GND                               GND ───┘  │  │                          GND ─┘  │  │
  │  └─────── A                         │  └─────── A                                   A ──────┘  │                            A ────┘  │
  └────────── B                         └────────── B                                   B ─────────┘                            B ───────┘
```



## Installation

just use the `esp32-example-jkpb-rs485.yaml` as proof of concept:

```
bash
# Install esphome
python3 -m pip install esphome

# Clone this external component
git clone https://github.com/txubelaxu/esphome-jk-bms.git
cd esphome-jk-bms

# Create a secrets.yaml containing some setup specific secrets
cat > secrets.yaml <<EOF
wifi_ssid: MY_WIFI_SSID
wifi_password: MY_WIFI_PASSWORD
EOF

There is 2 example configurations that you can change to your needs:
- esp32-example-jkpb-rs485_1master_1slave.yaml    <>  a config of 1 master and 1 slave  (total: 2 JK-PB BMSs)
- esp32-example-jkpb-rs485_1master_6slaves.yaml   <>  a config of 1 master and 6 slaves (total: 7 JK-PB BMSs)


# Configure every BMS you have in this yaml, assigning a name and the address of each one.



## VALIDATE CODE + UPLOAD + LAUNCH + VIEW LOGS:

# Validate the configuration, create a binary, upload it, and start logs
esphome run esp32-example-jkpb-rs485_1master_1slave.yaml

(previous example has 1 MASTER at address 0x00 and 1 SLAVE at address 0x01). There is another example with 6 slaves.


## Known issues

* ??

## TO DO

* Communication to INVERTER SYSTEM.


## Goodies

A user of this project ([@dr3amr](https://github.com/dr3amr)) shared some [Home Assistant Lovelace UI cards for a beautiful dashboard here](https://github.com/syssi/esphome-jk-bms/discussions/230).

![Custom Lovelace UI cards](images/lovelace-cards-contribution.jpg "Home Assistant Lovelace UI cards")

## Debugging

If this component doesn't work out of the box for your device please update your configuration to enable the debug output of the UART component and increase the log level to the see outgoing and incoming serial traffic:

logger:
  level: DEBUG

```

## MUCH MORE
Added a <a href="./home_assistant_dashboards/">Home Assistant Dashboard</a>

![image](https://github.com/txubelaxu/esphome-jk-bms/assets/156140720/5d5197c8-dbb4-4b0e-ae9f-0a66b88c2f82)
![image](https://github.com/txubelaxu/esphome-jk-bms/assets/156140720/b90b01e8-0833-444b-b122-b60c1fdcd92e)
![image](https://github.com/txubelaxu/esphome-jk-bms/assets/156140720/b74549aa-a7c8-4540-9392-82b8bfc3864a)
![image](https://github.com/txubelaxu/esphome-jk-bms/assets/156140720/9a295f65-4268-4769-8f47-146014cafacf)



## References

* https://secondlifestorage.com/index.php?threads/jk-b1a24s-jk-b2a24s-active-balancer.9591/
* https://github.com/jblance/jkbms
* https://github.com/jblance/mpp-solar/issues/112
* https://github.com/jblance/mpp-solar/blob/master/mppsolar/protocols/jk232.py
* https://github.com/jblance/mpp-solar/blob/master/mppsolar/protocols/jk485.py
* https://github.com/sshoecraft/jktool
* https://github.com/Louisvdw/dbus-serialbattery/blob/master/etc/dbus-serialbattery/jkbms.py
* https://blog.ja-ke.tech/2020/02/07/ltt-power-bms-chinese-protocol.html
