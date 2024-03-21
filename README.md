# esphome-jk-bms

![GitHub actions](https://github.com/syssi/esphome-jk-bms/actions/workflows/ci.yaml/badge.svg)
![GitHub stars](https://img.shields.io/github/stars/syssi/esphome-jk-bms)
![GitHub forks](https://img.shields.io/github/forks/syssi/esphome-jk-bms)
![GitHub watchers](https://img.shields.io/github/watchers/syssi/esphome-jk-bms)
[!["Buy Me A Coffee"](https://img.shields.io/badge/buy%20me%20a%20coffee-donate-yellow.svg)](https://www.buymeacoffee.com/txubelaxu)

ESPHome components to monitor Jikong Battery Management Systems (JK-BMS) via UART-TTL, BLE, and RS-485.

## Via RS-485 (best for monitoring)

**NEW:** monitor your new JK-PBx BMSs via internal RS485 network using 1 only ESP. [Click here](https://github.com/txubelaxu/esphome-jk-bms/blob/main/components/jk_rs485_bms/README.md) for more info. Home Assistant dashboards inside as well.

Last news: new parameters added and new dashboards
![image](https://github.com/txubelaxu/esphome-jk-bms/assets/156140720/9c3f7466-5e62-4667-bb70-c573c3a344e2)
![image](https://github.com/txubelaxu/esphome-jk-bms/assets/156140720/a02e1d83-54c4-4057-bf0d-d159917e52c8)
![image](https://github.com/txubelaxu/esphome-jk-bms/assets/156140720/5187ac3f-950a-40f9-ba00-097275ffe9bf)

## Via Bluetooth (BLE) or UART (best for controlling)

ESPHome component to control and monitor a Jikong Battery Management System (JK-BMS) via UART-TTL or BLE [Click here](https://github.com/txubelaxu/esphome-jk-bms/README_uart_ble.md) for more info.


## References
* https://www.youtube.com/@OffGridGarageAustralia
* https://github.com/syssi/esphome-jk-bms
* https://secondlifestorage.com/index.php?threads/jk-b1a24s-jk-b2a24s-active-balancer.9591/
* https://github.com/jblance/jkbms
* https://github.com/jblance/mpp-solar/issues/112
* https://github.com/jblance/mpp-solar/blob/master/mppsolar/protocols/jk232.py
* https://github.com/jblance/mpp-solar/blob/master/mppsolar/protocols/jk485.py
* https://github.com/sshoecraft/jktool
* https://github.com/Louisvdw/dbus-serialbattery/blob/master/etc/dbus-serialbattery/jkbms.py
* https://blog.ja-ke.tech/2020/02/07/ltt-power-bms-chinese-protocol.html
