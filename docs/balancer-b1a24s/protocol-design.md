# JK-Balancer protocol design

## Commands

### Get status

```
# Get status
>>> 55:AA:01:FF:00:00:FF
<<< EB:90:01:FF:16:11:0C:FB:11:0E:00:00:01:00:03:00:00:00:0B:07:D0:01:11:0C:F9:0C:FB:0C:FB:0C:FB:0C:FB:0C:FC:0C:FB:0C:FB:0C:FB:0C:FC:0C:FB:0C:FB:0C:FB:0C:FB:0C:FC:0C:FB:0C:FB:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:A3:DB
```

| Offset | Content | Data Type | Length | Unit | Meaning |
|--------|---------|-----------|--------|------|----------|
| 0 | Frame Header (0xEB 0x90) | uint8 | 2 | - | Start of frame |
| 2 | Slave Address (0x01) | uint8 | 1 | - | Address of slave |
| 3 | Command Code (0xFF) | uint8 | 1 | - | Command code |
| 4 | Total Battery Voltage (0x1ED3 = 78.910V) | uint16 | 1 | 10mV | Battery pack voltage |
| 6 | Average Cell Voltage (0x0F69 = 3.995V) | uint16 | 1 | mV | Average voltage per cell |
| 8 | Number of Detected Cells (0x14 = 20) | uint8 | 1 | count | Number of cells detected |
| 9 | Highest Cell (0x13 = Cell 19) | uint8 | 1 | - | Cell with highest voltage |
| 10 | Lowest Cell (0x02 = Cell 2) | uint8 | 1 | - | Cell with lowest voltage |
| 11 | Balance Status | uint8 | 1 | - | BIT0=Balance charging, BIT1=Balance discharging |
| 12 | Alarm Status | uint8 | 1 | - | BIT0=Wrong cell count, BIT1=Resistance too high, BIT2=Overvoltage |
| 13 | Maximum Voltage Difference (0x0007 = 7mV) | uint16 | 1 | mV | Difference between highest and lowest cell |
| 15 | Balance Current (0x0000 = 0mA) | uint16 | 1 | mA | Current balancing current |
| 17 | Balance Trigger Voltage (0x0005 = 5mV) | uint16 | 1 | mV | Voltage difference to start balancing |
| 19 | Maximum Balance Current (0x03E8 = 1000mA) | uint16 | 1 | mA | Maximum allowed balancing current |
| 21 | Balance Switch (0x01 = On) | uint8 | 1 | - | Balancing enabled/disabled |
| 22 | Configured Cell Count (0x14 = 20) | uint8 | 1 | count | Number of cells configured |
| 23-71 | Voltage 0-23 (0x0F69 = 3945mV each) | uint16 | 1 | mV | Individual cell voltages |
| 71 | Temperature (0x0016 = 22°C) | int16 | 1 | °C | Temperature reading |
| 73 | Checksum (0x6F) | uint8 | 1 | - | Frame checksum |


### Set cell count

```
# Set cell count to 16 cells [2...24 Cells]
>>> 55:AA:01:F0:00:10:00
<<< EB:90:01:F0:00:10:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:7C
```

### Set trigger voltage

```
# Set trigger voltage to 10 mV [2...1000mV]
>>> 55:AA:01:F2:00:0A:FC
<<< EB:90:01:F2:00:0A:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:78
```

### Set balancing current

```
# Set balancing current to 500 mA [30...2000mA]
>>> 55:AA:01:F4:01:F4:E9
<<< EB:90:01:F4:01:F4:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:65
```
