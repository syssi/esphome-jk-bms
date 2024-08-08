# esphome-jk-bms

![GitHub actions](https://github.com/syssi/esphome-jk-bms/actions/workflows/ci.yaml/badge.svg)
![GitHub stars](https://img.shields.io/github/stars/syssi/esphome-jk-bms)
![GitHub forks](https://img.shields.io/github/forks/syssi/esphome-jk-bms)
![GitHub watchers](https://img.shields.io/github/watchers/syssi/esphome-jk-bms)
[!["Buy Me A Coffee"](https://img.shields.io/badge/buy%20me%20a%20coffee-donate-yellow.svg)](https://www.buymeacoffee.com/txubelaxu)

ESPHome components to monitor Jikong Battery Management Systems (JK-BMS) via UART-TTL, BLE, and RS-485.

## Via RS-485 (best for monitoring and some switch control (when ESP is acting as MASTER)

**NEW:** monitor your new JK-PBx BMSs via internal RS485 network using 1 only ESP. [Click here](https://github.com/txubelaxu/esphome-jk-bms/blob/main/components/jk_rs485_bms/README.md) for more info. Home Assistant dashboards inside as well.

**Last Change:** control your BMSs settings (Numbers): cell_smart_sleep_voltage, cell_undervoltage_protection, cell_undervoltage_protection_recovery, cell_overvoltage_protection, cell_overvoltage_protection_recovery, cell_balancing_trigger_voltage, cell_soc100_voltage, cell_soc0_voltage, cell_request_charge_voltage, cell_request_float_voltage, cell_power_off_voltage, cell_balancing_starting_voltage, max_charging_current, charging_overcurrent_protection_delay, charging_overcurrent_protection_recovery_delay, max_discharging_current, discharging_overcurrent_protection_delay, discharging_overcurrent_protection_recovery_delay, short_circuit_protection_delay, short_circuit_protection_recovery_delay, max_balancing_current, charging_overtemperature_protection, charging_overtemperature_protection_recovery, discharging_overtemperature_protection, discharging_overtemperature_protection_recovery, charging_lowtemperature_protection, charging_lowtemperature_protection_recovery, 
mos_overtemperature_protection, mos_overtemperature_protection_recovery, cell_count_settings, battery_capacity_total_settings, precharging_time_from_discharge

Broadcast is possible with those settings too.
Updated: 0master_2slave example & 0master_7slave example

Last: control your BMSs settings (Switches) "balancing", "charging" and "discharging" switches. New "broadcast mode" to easy switch on/off every BMS in the network.

Last: new parameters added and new dashboards
+ New Dashboards (summary, cells, settings, protection) available [here](https://github.com/txubelaxu/esphome-jk-bms/tree/main/components/jk_rs485_bms/home_assistant_dashboards)
+ Multiple alarm signals: BATTempSensor1Absent, BATTempSensor2Absent, BATTempSensor3Absent, BATTempSensor4Absent, BATTempSensor5Absent, BatOVP, BatUVP, BatteryOverTemp, CPUAuxCommuErr, CellOVP, CellQuantity, CellUVP, ChOCP, ChOTP, ChSCP, ChUTP, ChargeMOS, CurSensorErr, DchOCP, DchOTP, DchSCP, DischargeMOS, DischargeOnFailed, GPSDisconneted, MOSTempSensorAbsent, ModifyPWDinTime, MosOTP, PLCModuleAnomaly, TemperatureSensorAnomaly, WireRes (thanks to [@jblance and @jrventer](https://github.com/jblance/mpp-solar/issues/460)
+ difference between: "switch status" and "real working status". Useful for balancing, for example: balancer could be ON, but balancing can be on or off, depending on conditions...

![image](https://github.com/txubelaxu/esphome-jk-bms/assets/156140720/5d5197c8-dbb4-4b0e-ae9f-0a66b88c2f82)
![image](https://github.com/txubelaxu/esphome-jk-bms/assets/156140720/b90b01e8-0833-444b-b122-b60c1fdcd92e)
![image](https://github.com/txubelaxu/esphome-jk-bms/assets/156140720/b74549aa-a7c8-4540-9392-82b8bfc3864a)
![image](https://github.com/txubelaxu/esphome-jk-bms/assets/156140720/9a295f65-4268-4769-8f47-146014cafacf)


## Via Bluetooth (BLE) or UART (best for controlling)

Last: new parameters added and new dashboards
+ New information about device: vendorid, hardware_version, software_version, device_name, device_password
  So, if you lose your password of your jk bms you can recover using this.

ESPHome component to control and monitor a Jikong Battery Management System (JK-BMS) via UART-TTL or BLE [Click here](https://github.com/txubelaxu/esphome-jk-bms/README_uart_ble.md) for more info.


Support
If you want to tip me for this work, you can now buy me a coffee

[!["Buy Me A Coffee"](https://img.shields.io/badge/buy%20me%20a%20coffee-donate-yellow.svg)](https://www.buymeacoffee.com/txubelaxu)


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
