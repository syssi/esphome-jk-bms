# esphome-jk-bms-can

![GitHub stars](https://img.shields.io/github/stars/Uksa007/esphome-jk-bms-can)
![GitHub forks](https://img.shields.io/github/forks/Uksa007/esphome-jk-bms-can)
![GitHub watchers](https://img.shields.io/github/watchers/Uksa007/esphome-jk-bms-can)

ESPHome component to monitor a Jikong Battery Management System (JK-BMS) via RS485 or BLE

This fork supports CAN bus commuinaction with inverters supporting the CANBUS Protocol compatible with Pylontech V1.3 and Goodwe V1.5.

It communicates with the JK-BMS using the RS485 port which is in fact not RS485, it is 3.3V TTL so it can be directly connected to the ESP32.
The ESP32 then sends the required CAN bus data to the inverter.

I have been testing using the Goodwe SECU-A5.4L battery profile selected in the inverter.<br>
Each SECA-A5.4 battery profile has 5.4kWh of storage, so select the number that is the closest match to your battery's total capacity.<br>

V1.01 12/10/22 Now being used with live inverter. Updates to add more inverter control when turning on/off charging and discharging
**Note:- CAN bus support is still in development and testing!!!**

Thanks go to syssi for help and making the original RS485 code! Buy him a coffee here https://www.buymeacoffee.com/syssi)


* Connect a TJA1050 to an ESP32 as per https://esphome.io/components/canbus.html?highlight=can#wiring-options
* Use this code in esphome https://github.com/Uksa007/esphome-jk-bms-can/blob/main/esp32-example-can.yaml
* A second ESP32 and TJA1050 make a test CAN bus/receiver https://github.com/Uksa007/esphome-jk-bms-can/blob/main/test-esp32-receiver-can.yaml
* If you don't yet have TJA1050 you can still test with two ESP32 by making a makeshif CAN bus DO NOT CONNECT TO A REAL CAN BUS!
  Add a blocking diode between the CAN-TX and CAN-RX pins (cathode towards the CAN-TX pin) at both ESP32s!
  So a Diode between GPIO5, GPIO4 diode cathode to GPIO5 at both ESP32
  Then tie the CAN-RX lines together with a pull-up resistor to 3.3V I used 10K pullup.
  
 Sample CAN messages sent:<br>

    can id: 0x359 hex: 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x22
    can id: 0x351 hex: 0x28, 0x02, 0xE8, 0x03, 0xE8, 0x03, 0xF7, 0x01
    can id: 0x355 hex: 0x1A, 0x00, 0x64, 0x00
    can id: 0x356 hex: 0x00, 0x00, 0x00, 0x00, 0xFA, 0x00
    can id: 0x35C hex: 0xC0
    can id: 0x35E hex: 0x45, 0x6E, 0x65, 0x72, 0x67, 0x79

 

![Lovelace entities card](lovelace-entities-card.png "Lovelace entities card")

## Supported devices

All JK-BMS models with software version `>=6.0` are using the implemented protocol and should be supported.

* JK-BD6A17S6P, hw 7.2, sw 7.1.0H
* JK-BD6A17S8P, hw 9.x, sw 9.01G (reported by [@jonadis](https://github.com/syssi/esphome-jk-bms/issues/35#issuecomment-1035312712))
* JK-BD6A24S6P, hw 6.x, sw 6.10S (reported by [@ziporah](https://github.com/syssi/esphome-jk-bms/issues/41))
* JK-BD6A24S10P, hw 8.x, sw 8.0.6G (reported by [@spoonwzd](https://github.com/syssi/esphome-jk-bms/issues/67#issuecomment-1093844076))
* JK-B1A24S15P, hw 8.x, sw 8.1.0H (reported by [@killee](https://github.com/syssi/esphome-jk-bms/discussions/4))
* JK-B1A20S15P, hw 8.x, sw 8.14U (reported by  [@trippfam07](https://github.com/syssi/esphome-jk-bms/issues/31))
* JK-B2A24S15P, hw 6.x, sw 6.1.3S (reported by [@miguel300477](https://github.com/syssi/esphome-jk-bms/issues/57))
* JK-B2A24S15P, hw 8.x, sw 8.21W (reported by [@mariusvaida](https://github.com/syssi/esphome-jk-bms/issues/120))
* JK-B2A24S15P, hw 10.xw, sw 10.07
* JK-B2A24S20P, hw 8.x, sw 8.1.2H (reported by [@KlausLi](https://github.com/syssi/esphome-jk-bms/issues/15#issuecomment-961447064))
* JK-B2A24S20P, hw 8.x, sw 8.20G (reported by [@rob-oravec](https://github.com/syssi/esphome-jk-bms/discussions/46))
* JK-B2A24S20P, hw 10.X-W, sw 10.02 (reported by [@SeByDocKy](https://github.com/syssi/esphome-jk-bms/issues/37#issuecomment-1040569576))
* JK-B2A24S20P, hw 10.XG, sw 10.07D30 (reported by [@TheSmartGerman](https://github.com/syssi/esphome-jk-bms/discussions/122))
* JK-B2A8S20P,  hw 9.x, sw 9.01M3 (reported by [@EasilyBoredEngineer](https://github.com/syssi/esphome-jk-bms/discussions/110))

## Untested devices

* JK-BD6A20S6P
* JK-BD6A20S10P

## Requirements

* [ESPHome 1.18.0 or higher](https://github.com/esphome/esphome/releases).
* Generic ESP32 or ESP8266 board

## Schematics

<a href="https://raw.githubusercontent.com/syssi/esphome-jk-bms/main/images/circuit.jpg" target="_blank">
<img src="https://raw.githubusercontent.com/syssi/esphome-jk-bms/main/images/circuit_thumbnail.jpg" width="50%">
</a>


```
                RS485-TTL
┌──────────┐                ┌─────────┐
│          │<----- RX ----->│         │
│  JK-BMS  │<----- TX ----->│ ESP32/  │
│          │<----- GND ---->│ ESP8266 │<-- 3.3V
│          │                │         │<-- GND
└──────────┘                └─────────┘

# RS485-TTL jack (4 Pin, JST 1.25mm pitch)
┌─── ─────── ────┐
│                │
│ O   O   O   O  │
│GND  RX  TX VBAT│
└────────────────┘
  │   │   │
  │   │   └─── GPIO17 (`rx_pin`)
  │   └─────── GPIO16 (`tx_pin`)
  └─────────── GND
```


The RS485-TTL jack of the BMS can be attached to any UART pins of the ESP. A hardware UART should be preferred because of the high baudrate (115200 baud).

## Installation

You can install this component with [ESPHome external components feature](https://esphome.io/components/external_components.html) like this:
```yaml
external_components:
  - source: github://syssi/esphome-jk-bms@main
```

or just use the `esp32-example.yaml` as proof of concept:

```bash
# Install esphome
pip3 install esphome

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

# Validate the configuration, create a binary, upload it, and start logs
# If you use a esp8266 run the esp8266-examle.yaml
esphome run esp32-example.yaml

```

## Example response all sensors enabled

```
[sensor:127]: 'jk-bms cell voltage 1': Sending state 4.12500 V with 3 decimals of accuracy
[sensor:127]: 'jk-bms cell voltage 2': Sending state 4.12500 V with 3 decimals of accuracy
[sensor:127]: 'jk-bms cell voltage 3': Sending state 4.12800 V with 3 decimals of accuracy
[sensor:127]: 'jk-bms cell voltage 4': Sending state 4.12400 V with 3 decimals of accuracy
[sensor:127]: 'jk-bms cell voltage 5': Sending state 4.12500 V with 3 decimals of accuracy
[sensor:127]: 'jk-bms cell voltage 6': Sending state 4.12800 V with 3 decimals of accuracy
[sensor:127]: 'jk-bms cell voltage 7': Sending state 4.12400 V with 3 decimals of accuracy
[sensor:127]: 'jk-bms cell voltage 8': Sending state 4.12300 V with 3 decimals of accuracy
[sensor:127]: 'jk-bms cell voltage 9': Sending state 4.12800 V with 3 decimals of accuracy
[sensor:127]: 'jk-bms cell voltage 10': Sending state 4.12800 V with 3 decimals of accuracy
[sensor:127]: 'jk-bms cell voltage 11': Sending state 4.12800 V with 3 decimals of accuracy
[sensor:127]: 'jk-bms cell voltage 12': Sending state 4.13100 V with 3 decimals of accuracy
[sensor:127]: 'jk-bms cell voltage 13': Sending state 4.12400 V with 3 decimals of accuracy
[sensor:127]: 'jk-bms power tube temperature': Sending state 24.00000 °C with 0 decimals of accuracy
[sensor:127]: 'jk-bms temperature sensor 1': Sending state 22.00000 °C with 0 decimals of accuracy
[sensor:127]: 'jk-bms temperature sensor 2': Sending state 22.00000 °C with 0 decimals of accuracy
[sensor:127]: 'jk-bms total voltage': Sending state 53.64000 V with 2 decimals of accuracy
[sensor:127]: 'jk-bms current': Sending state -0.00000 A with 2 decimals of accuracy
[sensor:127]: 'jk-bms capacity remaining': Sending state 99.00000 % with 0 decimals of accuracy
[sensor:127]: 'jk-bms temperature sensors': Sending state 2.00000  with 0 decimals of accuracy
[sensor:127]: 'jk-bms charging cycles': Sending state 0.00000  with 0 decimals of accuracy
[sensor:127]: 'jk-bms total charging cycle capacity': Sending state 0.00000  with 0 decimals of accuracy
[sensor:127]: 'jk-bms battery strings': Sending state 13.00000  with 0 decimals of accuracy
[sensor:127]: 'jk-bms errors bitmask': Sending state 0.00000  with 0 decimals of accuracy
[text_sensor:015]: 'jk-bms errors': Sending state ''
[sensor:127]: 'jk-bms operation mode bitmask': Sending state 0.00000  with 0 decimals of accuracy
[text_sensor:015]: 'jk-bms operation mode': Sending state ''
[sensor:127]: 'jk-bms total voltage overvoltage protection': Sending state 5.46000 V with 3 decimals of accuracy
[sensor:127]: 'jk-bms total voltage undervoltage protection': Sending state 3.77000 V with 3 decimals of accuracy
[sensor:127]: 'jk-bms cell voltage overvoltage protection': Sending state 4.20000 V with 3 decimals of accuracy
[sensor:127]: 'jk-bms cell voltage overvoltage recovery': Sending state 4.10000 V with 3 decimals of accuracy
[sensor:127]: 'jk-bms cell voltage overvoltage delay': Sending state 5.00000 s with 0 decimals of accuracy
[sensor:127]: 'jk-bms cell voltage undervoltage protection': Sending state 2.90000 V with 3 decimals of accuracy
[sensor:127]: 'jk-bms cell voltage undervoltage recovery': Sending state 3.20000 V with 3 decimals of accuracy
[sensor:127]: 'jk-bms cell voltage undervoltage delay': Sending state 5.00000 s with 0 decimals of accuracy
[sensor:127]: 'jk-bms cell pressure difference protection': Sending state 0.30000 V with 3 decimals of accuracy
[sensor:127]: 'jk-bms discharging overcurrent protection': Sending state 60.00000 A with 0 decimals of accuracy
[sensor:127]: 'jk-bms discharging overcurrent delay': Sending state 300.00000 s with 0 decimals of accuracy
[sensor:127]: 'jk-bms charging overcurrent protection': Sending state 25.00000 A with 0 decimals of accuracy
[sensor:127]: 'jk-bms charging overcurrent delay': Sending state 30.00000 s with 0 decimals of accuracy
[sensor:127]: 'jk-bms balance starting voltage': Sending state 3.30000 V with 3 decimals of accuracy
[sensor:127]: 'jk-bms balance opening pressure difference': Sending state 0.01000 V with 3 decimals of accuracy
[switch:045]: 'jk-bms balancing': Sending state ON
[sensor:127]: 'jk-bms power tube temperature protection': Sending state 90.00000 °C with 0 decimals of accuracy
[sensor:127]: 'jk-bms power tube temperature recovery': Sending state 70.00000 °C with 0 decimals of accuracy
[sensor:127]: 'jk-bms temperature sensor temperature protection': Sending state 100.00000 °C with 0 decimals of accuracy
[sensor:127]: 'jk-bms temperature sensor temperature recovery': Sending state 100.00000 °C with 0 decimals of accuracy
[sensor:127]: 'jk-bms temperature sensor temperature difference protection': Sending state 20.00000 °C with 0 decimals of accuracy
[sensor:127]: 'jk-bms charging high temperature protection': Sending state 70.00000 °C with 0 decimals of accuracy
[sensor:127]: 'jk-bms discharging high temperature protection': Sending state 70.00000 °C with 0 decimals of accuracy
[sensor:127]: 'jk-bms charging low temperature protection': Sending state -20.00000 °C with 0 decimals of accuracy
[sensor:127]: 'jk-bms charging low temperature recovery': Sending state -10.00000 °C with 0 decimals of accuracy
[sensor:127]: 'jk-bms discharging low temperature protection': Sending state -20.00000 °C with 0 decimals of accuracy
[sensor:127]: 'jk-bms discharging low temperature recovery': Sending state -10.00000 °C with 0 decimals of accuracy
[switch:045]: 'jk-bms charging': Sending state OFF
[switch:045]: 'jk-bms discharging': Sending state OFF
[sensor:127]: 'jk-bms current calibration': Sending state 0.72500 A with 3 decimals of accuracy
[sensor:127]: 'jk-bms device address': Sending state 1.00000  with 0 decimals of accuracy
[text_sensor:015]: 'jk-bms battery type': Sending state 'Ternary Lithium'
[sensor:127]: 'jk-bms sleep wait time': Sending state 10.00000 s with 0 decimals of accuracy
[sensor:127]: 'jk-bms alarm low volume': Sending state 20.00000  with 0 decimals of accuracy
[text_sensor:015]: 'jk-bms password': Sending state '123456'
[switch:045]: 'jk-bms dedicated charger': Sending state OFF
[text_sensor:015]: 'jk-bms device type': Sending state 'Input Us'
[sensor:127]: 'jk-bms total runtime': Sending state 0.00000 h with 0 decimals of accuracy
[text_sensor:015]: 'jk-bms software version': Sending state 'H7.X__S7.1.0H__'
[sensor:127]: 'jk-bms actual_battery_capacity': Sending state 186.00000 Ah with 0 decimals of accuracy
[text_sensor:015]: 'jk-bms manufacturer': Sending state 'BT3072020120000200521001'
[sensor:127]: 'jk-bms protocol version': Sending state 1.00000  with 0 decimals of accuracy
```

## Known issues

* The battery type sensor is pretty useless because the BMS reports always the same value (`Ternary Lithium`). Regardless of which battery type was set / parameter set was loaded via the android app. ([#9][i9])
* ESP32: Adding all supported sensors can lead to a stack overflow / boot loop. This can be solved by increasing the stack size. ([#63][i63])

[i9]: https://github.com/syssi/esphome-jk-bms/issues/9
[i63]: https://github.com/syssi/esphome-jk-bms/issues/63

## Debugging

If this component doesn't work out of the box for your device please update your configuration to enable the debug output of the UART component and increase the log level to the see outgoing and incoming serial traffic:

```
logger:
  level: DEBUG

uart:
  id: uart0
  baud_rate: 115200
  rx_buffer_size: 384
  tx_pin: GPIO14
  rx_pin: GPIO4
  debug:
    direction: BOTH
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
