# JK-BMS protocol design

## Request / Response example

```
>>> 4E 57 00 13 00 00 00 00 06 03 00 00 00 00 00 00 68 00 00 01 29
```

```
<<< 4E 57 01 1B 00 00 00 00 06 00 01
79 2A
01 0E ED
02 0E FA
03 0E F7
04 0E EC
05 0E F8
06 0E FA
07 0E F1
08 0E F8
09 0E E3
0A 0E FA
0B 0E F1
0C 0E FB
0D 0E FB
0E 0E F2
80 00 1D
81 00 1E
82 00 1C
83 14 EF
84 80 D0
85 0F
86 02
87 00 04
89 00 00 00 00
8A 00 0E
8B 00 00
8C 00 07
8E 16 26
8F 10 AE
90 0F D2
91 0F A0
92 00 05
93 0B EA
94 0C 1C
95 00 05
96 01 2C
97 00 07
98 00 03
99 00 05
9A 00 05
9B 0C E4
9C 00 08
9D 01
9E 00 5A
9F 00 46
A0 00 64
A1 00 64
A2 00 14
A3 00 46
A4 00 46
A5 FF EC
A6 FF F6
A7 FF EC
A8 FF F6
A9 0E
AA 00 00 00 0E
AB 01
AC 01
AD 04 11
AE 01
AF 01
B0 00 0A
B1 14
B2 31 32 33 34 35 36 00 00 00 00
B3 00
B4 49 6E 70 75 74 20 55 73
B5 32 31 30 31
B6 00 00 E2 00
B7 48 36 2E 58 5F 5F 53 36 2E 31 2E 33 53 5F 5F
B8 00
B9 00 00 00 00
BA 42 54 33 30 37 32 30 32 30 31 32 30 30 30 30 32 30 30 35 32 31 30 30 31
C0 01
00 00 00 00 68 00 00 54 D1
```

## Registers

| RW   | Code | Name                                                | Bytes | Type  | Info |
| ---: | :--- | :-------------------------------------------------- | :---: | :---: | :--- |
|    R | 0x79 | Individual Cell voltage                             |   3*n |  Hex  | The first byte is the cell number, the next two bytes is the voltage value MV. When reading all the data at the same time, 0x79 is followed by one byte length data, n as shown above, and then a group of three bytes represents the | electricity Cell voltage. |
|    R | 0x80 | Read power tube temperature                         |    2  |  Hex  | 0-140 (-40 to 100) The part exceeding 100 is negative temperature, such as 101 is negative 1 degree (100 Benchmark) |
|    R | 0x81 | Read the temperature in the battery box             |    2  |  Hex  | 0-140 (-40 to 100) The part exceeding 100 is negative temperature, the same as above (100 reference) |
|    R | 0x82 | Read battery temperature                            |    2  |  Hex  | 0-140 (-40 to 100) The part exceeding 100 is negative temperature, the same as above (100 reference) |
|    R | 0x83 | Total battery voltage                               |    2  |  Hex  | 0.01V 3500*0.01=35.00v minimum unit 10mV |
|    R | 0x84 | Current data                                        |    2  |  Hex  | 10000 (10000-11000)*0.01=-10.00 A (discharge) (10000-9500)*0.01=5.00 A (charging) Accuracy 10 mA unit: 0.01 A Note: C0:0x01 redefine 0x84 current data, the unit is 10 mA, the highest bit is 0 Means discharging, 1 means charging If discharging 20 A, the data transmitted will be 2000 (0x07D0) If charging 20 A, the transmission data is 34768 (0x87D0) |
|    R | 0x85 | Battery remaining capacity                          |    1  |  Hex  | SOC, 0-100%, |
|    R | 0x86 | Number of battery temperature sensors               |    1  |  Hex  | Two battery temperature sensors |
|    R | 0x87 | Number of battery cycles                            |    2  |  Hex  |  |
|    R | 0x89 | Total battery cycle capacity                        |    4  |  Hex  | Anshi |
|    R | 0x8a | Total number of battery strings                     |    2  |  Hex  |  |
|    R | 0x8b | Battery warning message                             |    2  |  Bitmask | See ^1 |
|    R | 0x8c | Battery status information                          |    2  |  Bitmask | 0 bit charging MOS tube state 1 on 0 off This is for upload prompt 1 bit discharge MOS tube state 1 on 0 off This is for upload prompts. 2-position balance switch state 1 on, 0 off, this is for uploading prompts 3 battery dropped 1 is normal. 0 is offline, this is the upload prompt, Bits 4-15: reserved example: 00 01: Indicates that the charging MOS tube is turned on |
|   RW | 0x8e | Total voltage overvoltage protection                |    2  |  Hex  | 1000-15000 (10 mV) Minimum unit 10mV |
|   RW | 0x8f | Total voltage undervoltage protection               |    2  |  Hex  | 1000-15000 (10 mV) Minimum unit 10mV |
|   RW | 0x90 | Single overvoltage protection voltage               |    2  |  Hex  | 1000-4500 mV |
|   RW | 0x91 | Cell overvoltage recovery voltage                   |    2  |  Hex  | 1000-4500 mV |
|   RW | 0x92 | Single overvoltage protection delay                 |    2  |  Hex  | 1-60 seconds |
|   RW | 0x93 | Single undervoltage protection voltage              |    2  |  Hex  | 1000-4500 mV |
|   RW | 0x94 | Monomer undervoltage recovery voltage               |    2  |  Hex  | 1000-4500 mV |
|   RW | 0x95 | Single undervoltage protection delay                |    2  |  Hex  | 1-60 seconds |
|   RW | 0x96 | Cell pressure difference protection value           |    2  |  Hex  | 0-1000 mV |
|   RW | 0x97 | Discharge overcurrent protection value              |    2  |  Hex  | 1-1000 A |
|   RW | 0x98 | Discharge overcurrent delay                         |    2  |  Hex  | 1-60 seconds |
|   RW | 0x99 | Charging overcurrent protection value               |    2  |  Hex  | 1-1000 A |
|   RW | 0x9a | Charge overcurrent delay                            |    2  |  Hex  | 1-60 seconds |
|   RW | 0x9b | Balanced starting voltage                           |    2  |  Hex  | 2000-4500 mV |
|   RW | 0x9c | Balanced opening pressure difference                |    2  |  Hex  | 10-1000 mV |
|   RW | 0x9d | Active balance switch                               |    1  |  Hex  | 0 (off), 1 (on) |
|   RW | 0x9e | Power tube temperature protection value             |    2  |  Hex  | 0-100 |
|   RW | 0x9f | Power tube temperature recovery value               |    2  |  Hex  | 0-100 |
|   RW | 0xa0 | Temperature protection value in the battery box     |    2  |  Hex  | 40-100 |
|   RW | 0xa1 | Temperature recovery value in the battery box       |    2  |  Hex  | 40-100 |
|   RW | 0xa2 | Battery temperature difference protection value     |    2  |  Hex  | 5-20 |
|   RW | 0xa3 | Battery charging high temperature protection value  |    2  |  Hex  | 0-100 |
|   RW | 0xa4 | Battery discharge high temperature protection value |    2  |  Hex  | 0-100 |
|   RW | 0xa5 | Charging low temperature protection value           |    2  |  Hex  | -45 /+25 (no reference-signed data) |
|   RW | 0xa6 | Charging low temperature protection recovery value  |    2  |  Hex  | -45 /+25 (no reference-signed data) |
|   RW | 0xa7 | Discharge low temperature protection value          |    2  |  Hex  | -45 /+25 (no reference-signed data) |
|   RW | 0xa8 | Discharge low temperature protection recovery value |    2  |  Hex  | -45 /+25 (no reference-signed data) |
|   RW | 0xa9 | Battery string setting                              |    1  |  Hex  | 3-32 |
|   RW | 0xaa | Battery capacity setting                            |    4  |  Hex  | Ah (Amp Hour) |
|   RW | 0xab | Charging MOS tube switch                            |    1  |  Hex  | 0 (off), 1 (open) |
|   RW | 0xac | Discharge MOS tube switch                           |    1  |  Hex  | 0 (off), 1 (open) |
|   RW | 0xad | Current calibration                                 |    2  |  Hex  | 100-20000 mA |
|   RW | 0xae | Protection board address                            |    1  |  Hex  | is reserved for use when cascading, |
|   RW | 0xaf | Battery Type                                        |    1  |  Hex  | 0 (lithium iron phosphate), 1 (ternary), 2 (lithium titanate) |
|   RW | 0xb0 | Sleep waiting time                                  |    2  |  Hex  | second data, temporarily for reference |
|   RW | 0xb1 | Low volume alarm value                              |    1  |  Hex  | 0-80 % |
|   RW | 0xb2 | Modify parameter password                           |   10  |  Hex  | is temporarily used as a reference, fix a password, |
|   RW | 0xb3 | Dedicated charger switch                            |    1  |  Hex  | 0 (off), 1 (on) |
|   RW | 0xb4 | Device ID code                                      |    8  |  Char | Example 60300001 (60-nominal voltage level: defined by the voltage level, for example, 60 is 60V. Series 48 is 48V series; 3-material system: according to the system definition of battery materials such as iron. Lithium code is 1 manganic acid code 2 ternary code 3; 00001-production serial number: according to manufacturing. The Nth group of the model produced by the manufacturer that month is numbered N (for example: a certain type The first group of the number, then N is 00001)) characters |
|   RW | 0xb5 | Date of manufacture                                 |    4  |  Char | Example 2004-production year: take the last two digits according to the actual production year; list the production in 2020, Battery, year code 20; Production month: 01-12 months; |
|   RW | 0xb6 | System working hours                                |    4  |  Hex  | is cleared when leaving factory, unit minute |
|    R | 0xb7 | Software version number                             |   15  |  Char | NW_1_0_0_200428 |
|   RW | 0xb8 | Whether to start current calibration                |    1  |  Hex  | 1 (Start calibration) 0 (Close calibration) |
|   RW | 0xb9 | Actual battery capacity                             |    4  |  Hex  | Ah (Amp Hour) |
|   RW | 0xba | Manufacturer ID naming                              |   24  |  Char | See ^2 |
|    W | 0xbb | Restart the system                                  |    1  |  Hex  | 1 (Restart the system) |
|    W | 0xbc | reset                                               |    1  |  Hex  | 1 (Restore) (only restore the factory reference parameters) |
|    W | 0xbd | Remote upgrade logo                                 |    1  |  Hex  | Start of Hex 1 (wait for logo response when issuing documents) |
|    W | 0xbe | Battery low voltage turns off GPS                   |    2  |  Hex  | unit: mV ( turn off the power to GPS when low voltage is detected ) |
|    W | 0xbf | Battery low voltage recovery GPS                    |    2  |  Hex  | unit: mV ( turn on the power to the GPS when the recovery voltage value is detected ) |
|    R | 0xc0 | Protocol version number                             |    1  |  Hex  | Default value: 0x00 0x01: Redefine 0x84 current data, the unit is 10 mA, and the highest bit is 0 for discharging. 1 means charging If discharging 20A, the data transmitted will be 2000 (0x07D0) If charging 20A, the transmission data is 34768 (0x87D0) |

*^1: Battery warning message

| Bit | Description                         | Values                | Severity |
| --- | ----------------------------------- | --------------------- | -------- |
|  0  | Low capacity alarm                  | 1 (alarm), 0 (normal) | warning  |
|  1  | MOS tube over-temperature alarm     | 1 (alarm), 0 (normal) | alarm    |
|  2  | Charging over-voltage alarm         | 1 (alarm), 0 (normal) | alarm    |
|  3  | Discharge undervoltage alarm        | 1 (alarm), 0 (normal) | alarm    |
|  4  | Battery over temperature alarm      | 1 (alarm), 0 (normal) | alarm    |
|  5  | charging overcurrent alarm          | 1 (alarm), 0 (normal) | alarm    |
|  6  | Discharge overcurrent alarm         | 1 (alarm), 0 (normal) | alarm    |
|  7  | Cell pressure difference alarm      | 1 (alarm), 0 (normal) | alarm    |
|     |                                     |                       |          |
|  8  | Over-temperature alarm in the battery box | 1 (alarm), 0 (normal) | alarm    |
|  9  | Battery low temperature alarm       | 1 (alarm), 0 (normal) | alarm    |
| 10  | Monomer overvoltage alarm           | 1 (alarm), 0 (normal) | alarm    |
| 11  | Monomer undervoltage alarm          | 1 (alarm), 0 (normal) | alarm    |
| 12  | 309_A protection                    | 1 (alarm), 0 (normal) | alarm    |
| 13  | 309_A protection                    | 1 (alarm), 0 (normal) | alarm    |
| 14  | Reserved                            |                       |          |
| 15  | Reserved                            |                       |          |

**Example**

0x0001 = 00000000 00000001: Low capacity alarm \
0x0002 = 00000000 00000010: MOS tube over-temperature alarm \
0x0003 = 00000000 00000011: Low capacity alarm AND MOS tube over-temperature alarm


*^2: Manufacturer ID naming (24 chars)

Example value: BT3072020120000200521001 -> BT 3 072 020 12 0000 20 05 21 001

| Value | Description       | Known values                                              |
| ----- | ----------------- | --------------------------------------------------------- |
| BT    | Product name      | BT for battery                                            |
| 3     | Battery type      | 1 (Iron-Lithium), 2 (Mangenese acid), 3 (Ternary code)    |
| 072   | Voltage level     | 48V, 60V, 72V                                             |
| 020   | Capacity grade    | 20 for 20AH                                               |
| 12    | Cycle life        | 4 (400 cycles), 12 (1200 cycles)                          |
| 0000  | Manufacturer code |                                                           |
| 20    | Production year   | 20 (2020), 21 (2021)                                      |
| 05    | Production month  | 1-12                                                      |
| 21    | Production day    | 1-31                                                      |
| 0001  | Production serial number | According to the Nth group of the model produced on the day of the manufacturer’s production date, The number is N (for example: the first group of a certain model, then 001) |

