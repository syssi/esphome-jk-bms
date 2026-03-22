# JK BMS BLE Protocol Documentation

## BLE Service & Characteristic

The BMS is accessible via the following BLE service:
- Service UUID: `0xFFE0`
- Write Characteristic UUID: `0xFFE1` (handle `0x03`) - Used to send commands
- Notification Characteristic UUID: `0xFFE1` (handle `0x05`) - Used to receive notifications

**Note:** While both characteristics share the same UUID (`0xFFE1`), they have different handles and purposes:
- Commands are written to handle `0x03`
- Notifications are received on handle `0x05`

## Communication Flow

1. Establish BLE connection
2. Register for notifications on characteristic `0xFFE1` (handle `0x05`)
3. Send `0x96` on characteristic `0xFFE1` (handle `0x03`) → BMS responds with Frame 0x01 (settings)
4. Send `0x97` on characteristic `0xFFE1` (handle `0x03`) → BMS responds with Frame 0x03 (device info)
5. After both commands are acknowledged, BMS automatically streams Frame 0x02 (cell info) periodically

## Command Structure

Commands are sent as 20-byte frames with the following format:

| Offset | Length | Description |
|-------:|-------:|-------------|
|    0-3 |      4 | Header (0xAA 0x55 0x90 0xEB) |
|      4 |      1 | Command byte |
|      5 |      1 | Length byte |
|   6-15 |     10 | Data (command-specific payload) |
|     16 |      1 | Counter (sequence counter, wraps at 0xFF) |
|  17-18 |      2 | Reserved (0x00 0x00) |
|     19 |      1 | CRC |

### Command Types

- `0x97`: Device info request → triggers Frame 0x03 (device info)
- `0x96`: Settings request → triggers Frame 0x01 (settings); Frame 0x02 (cell data) auto-streams once both `0x96` and `0x97` have been sent

### Example Command Payloads

#### Device Info Command (0x97)
```
AA 55 90 EB     # Header
97              # Command
00              # Length
00 00 00 00     # Padding
00 00 00 00     # Padding
00 00           # Padding
XX              # Counter (sequence)
00 00           # Reserved
00              # CRC (sum8 of bytes 0-18)
```

#### Cell Info Command (0x96)
```
AA 55 90 EB     # Header
96              # Command
00              # Length
00 00 00 00     # Padding
00 00 00 00     # Padding
00 00           # Padding
XX              # Counter (sequence)
00 00           # Reserved
00              # CRC (sum8 of bytes 0-18)
```

## Response Handling

### Frame Assembly
- Responses are at least 300 bytes; some devices send up to 320 bytes
- Responses are fragmented due to BLE MTU limitations
- Each response frame starts with sequence: `0x55 0xAA 0xEB 0x90`
- When start sequence is detected, current frame buffer is cleared
- Frame validation uses CRC (sum of all bytes except CRC byte)
- All responses are received as notifications on characteristic `0xFFE1` (handle `0x05`)

### Response Frame Types
Response type is identified by byte[4]:
- `0x01`: Settings frame
- `0x02`: Cell info frame  
- `0x03`: Device info frame

## CRC Calculation

The CRC is calculated by summing all bytes in the frame except the CRC byte itself:

```c
uint8_t crc(const uint8_t data[], const uint16_t len) {
  uint8_t crc = 0;
  for (uint16_t i = 0; i < len; i++) {
    crc = crc + data[i];
  }
  return crc;
}
```

## Error Handling

- Maximum allowed frame size is 320 bytes
- Frames exceeding this size are dropped
- CRC validation is performed on assembled frames
- Frames with invalid CRC are discarded

## Frame Types

### Cell Info Frame Specification (`JK02_32S`)

| Position (Bytes) | Length | Description | Format | Coefficient | Unit | Notes |
|-----------------:|-------:|-------------|--------|-------------|------|-------|
|              0-3 |      4 | Header | Raw | - | - | Fixed value: 0x55 0xAA 0xEB 0x90 |
|                4 |      1 | Record type | uint8 | - | - | Fixed value: 0x02 |
|                5 |      1 | Frame counter | uint8 | - | - | - |
|             6-69 |     64 | Cell voltages 1-32 | uint16 | 0.001 | V | 2 bytes per cell |
|            70-73 |      4 | Enabled cells bitmask | Raw | - | - | 32-bit LE; bit0=cell1 … bit31=cell32 |
|            74-75 |      2 | Average cell voltage | uint16 | 0.001 | V | Present in frame; ESPHome computes locally |
|            76-77 |      2 | Delta cell voltage | uint16 | 0.001 | V | Present in frame; ESPHome computes locally |
|               78 |      1 | Max voltage cell number | uint8 | 1 | - | Present in frame; ESPHome computes locally |
|               79 |      1 | Min voltage cell number | uint8 | 1 | - | Present in frame; ESPHome computes locally |
|           80-143 |     64 | Cell resistances 1-32 | uint16 | 0.001 | Ohm | 2 bytes per cell |
|          144-145 |      2 | Power tube temperature | int16 | 0.1 | °C | - |
|          146-149 |      4 | Wire resistance warning bitmask | Raw | - | - | 32-bit LE; bit0=cell1 … bit31=cell32 |
|          150-153 |      4 | Battery voltage | uint32 | 0.001 | V | - |
|          154-157 |      4 | Battery power | uint32 | 0.001 | W | Raw field; ESPHome computes signed power from voltage × current instead |
|          158-161 |      4 | Charge current | int32 | 0.001 | A | - |
|          162-163 |      2 | Temperature Sensor 1 | int16 | 0.1 | °C | - |
|          164-165 |      2 | Temperature Sensor 2 | int16 | 0.1 | °C | - |
|          166-167 |      2 | Errors bitmask | uint16 | - | - | See errors bitmask notes below |
|          168-169 |      2 | (Reserved in JK02_32S) | - | - | - | JK02_24S system alarm position; unused in JK02_32S |
|          170-171 |      2 | Balance current | int16 | 0.001 | A | - |
|              172 |      1 | Balancing action | Raw | - | - | 0=Off, 1=Charging balancer, 2=Discharging balancer |
|              173 |      1 | State of charge | uint8 | 1 | % | - |
|          174-177 |      4 | Remaining capacity | uint32 | 0.001 | Ah | - |
|          178-181 |      4 | Nominal capacity | uint32 | 0.001 | Ah | - |
|          182-185 |      4 | Cycle count | uint32 | 1 | - | - |
|          186-189 |      4 | Total cycle capacity | uint32 | 0.001 | Ah | - |
|              190 |      1 | State of Health (SOH) | uint8 | 1 | % | - |
|              191 |      1 | Precharge status | Raw | - | - | 0=Off, 1=On |
|          192-193 |      2 | User alarm | uint16 | - | - | Logged only |
|          194-197 |      4 | Total runtime | uint32 | 1 | s | - |
|              198 |      1 | Charging MOSFET status | Raw | - | - | 0=Off, 1=On |
|              199 |      1 | Discharging MOSFET status | Raw | - | - | 0=Off, 1=On |
|              200 |      1 | Precharging status | Raw | - | - | 0=Off, 1=On |
|              201 |      1 | Balancing indicator | Raw | - | - | 0=Off, 1=On; logged only |
|          202-203 |      2 | Discharge overcurrent protection release timer | uint16 | 1 | s | Logged only |
|          204-205 |      2 | Discharge short circuit protection release timer | uint16 | 1 | s | Logged only |
|          206-207 |      2 | Charge overcurrent protection release timer | uint16 | 1 | s | Logged only |
|          208-209 |      2 | Charge short circuit protection release timer | uint16 | 1 | s | Logged only |
|          210-211 |      2 | Undervoltage protection release timer | uint16 | 1 | s | Logged only |
|          212-213 |      2 | Overvoltage protection release timer | uint16 | 1 | s | Logged only |
|              214 |      1 | Temperature sensor absent bitmask | uint8 | - | - | Logged only; bit0=MOS, bit1-5=Sensors 1-5 |
|              215 |      1 | Heating status | Raw | - | - | 0=Off, 1=On |
|          216-217 |      2 | Reserved | - | - | - | - |
|          218-219 |      2 | Emergency time countdown | uint16 | 1 | s | >0 = Emergency active |
|          254-255 |      2 | Temperature Sensor 5 | int16 | 0.1 | °C | - |
|          256-257 |      2 | Temperature Sensor 4 | int16 | 0.1 | °C | - |
|          258-259 |      2 | Temperature Sensor 3 | int16 | 0.1 | °C | - |
|          278-279 |      2 | Charge status time elapsed | uint16 | 1 | s | JK02_32S only |
|              280 |      1 | Charge status ID | uint8 | - | - | JK02_32S only; 0=Bulk, 1=Absorption, 2=Float |
|              281 |      1 | Dry contact switch status | Raw | - | - | JK02_32S only; see dry contact bitmask notes below |
|              299 |      1 | CRC checksum | uint8 | - | - | - |

Errors bitmask notes for bytes 166-167 (big-endian; bit0 = LSB of byte 167):
- bit0: Charge overtemperature
- bit1: Charge undertemperature
- bit2: Coprocessor communication error
- bit3: Cell undervoltage
- bit4: Battery pack undervoltage
- bit5: Discharge overcurrent
- bit6: Discharge short circuit
- bit7: Discharge overtemperature
- bit8: Wire resistance
- bit9: MOSFET overtemperature
- bit10: Cell count mismatch
- bit11: Current sensor anomaly
- bit12: Cell overvoltage
- bit13: Battery pack overvoltage
- bit14: Charge overcurrent protection
- bit15: Charge short circuit

Dry contact bitmask notes for byte 281 (JK02_32S only):
- bit1 (0x02): DRY1 active
- bit2 (0x04): DRY2 active

### Device Info Frame Specification (`JK02_32S`)

| Position (Bytes) | Length | Description | Format | Coefficient | Unit | Notes |
|-----------------:|-------:|-------------|--------|-------------|------|-------|
|              0-3 |      4 | Frame header | hex | - | - | Fixed value: 0x55 0xAA 0xEB 0x90 |
|                4 |      1 | Frame type | uint8 | - | - | Fixed value: 0x03 |
|                5 |      1 | Frame counter | uint8 | - | - | Increments with each frame |
|             6-21 |     16 | Vendor ID | string | - | - | Null-terminated string (16 bytes including null) |
|            22-29 |      8 | Hardware version | string | - | - | Null-terminated string |
|            30-37 |      8 | Software version | string | - | - | Null-terminated string |
|            38-41 |      4 | Device uptime | uint32 | 1 | seconds | Little-endian |
|            42-45 |      4 | Power on count | uint32 | 1 | count | Little-endian |
|            46-61 |     16 | Device name | string | - | - | Null-terminated string |
|            62-77 |     16 | Device passcode | string | - | - | Null-terminated string |
|            78-85 |      8 | Manufacturing date | string | - | - | Null-terminated string |
|           86-101 |     16 | Serial number | string | - | - | Null-terminated string |
|          102-117 |     16 | User data | string | - | - | Null-terminated string |
|          118-133 |     16 | Setup passcode | string | - | - | Null-terminated string |
|              184 |      1 | UART1M Protocol number | uint8 | - | - | Protocol version |
|              185 |      1 | CAN Protocol number | uint8 | - | - | Protocol version |
|              218 |      1 | UART2M Protocol number | uint8 | - | - | Protocol version |
|              219 |      1 | UART2M Protocol enable | uint8 | - | - | Enable flag |
|              234 |      1 | LCD buzzer trigger | uint8 | - | - | Trigger setting |
|              235 |      1 | DRY1 Trigger | uint8 | - | - | Trigger setting |
|              236 |      1 | DRY2 Trigger | uint8 | - | - | Trigger setting |
|              237 |      1 | UART protocol library version | uint8 | - | - | Protocol version |
|          238-241 |      4 | LCD Buzzer Trigger Value | uint32 | - | - | Little-endian |
|          242-245 |      4 | LCD Buzzer Release Value | uint32 | - | - | Little-endian |
|          246-249 |      4 | DRY1 Trigger Value | uint32 | - | - | Little-endian |
|          250-253 |      4 | DRY1 Release Value | uint32 | - | - | Little-endian |
|          254-257 |      4 | DRY2 Trigger Value | uint32 | - | - | Little-endian |
|          258-261 |      4 | DRY2 Release Value | uint32 | - | - | Little-endian |
|          262-265 |      4 | Data Stored Period | uint32 | - | - | Little-endian |
|              266 |      1 | RCV Time | uint8 | 0.1 | hours | Request Charge Voltage Time |
|              267 |      1 | RFV Time | uint8 | 0.1 | hours | Request Float Voltage Time |
|              268 |      1 | CAN protocol library version | uint8 | - | - | Protocol version |
|              299 |      1 | CRC checksum | uint8 | - | - | - |

### Settings Frame Specification (`JK02_32S`)

| Position (Bytes) | Length | Description | Format | Coefficient | Unit | Notes |
|-----------------:|-------:|-------------|--------|-------------|------|-------|
|              0-3 |      4 | Header | hex | - | - | 0x55 0xAA 0xEB 0x90 |
|                4 |      1 | Frame type | hex | - | - | 0x01 |
|                5 |      1 | Frame counter | uint8 | - | - | - |
|              6-9 |      4 | Smart sleep voltage | uint32 | 0.001 | V | - |
|            10-13 |      4 | Cell UVP (undervoltage protection) | uint32 | 0.001 | V | - |
|            14-17 |      4 | Cell UVPR (undervoltage protection recovery) | uint32 | 0.001 | V | - |
|            18-21 |      4 | Cell OVP (overvoltage protection) | uint32 | 0.001 | V | - |
|            22-25 |      4 | Cell OVPR (overvoltage protection recovery) | uint32 | 0.001 | V | - |
|            26-29 |      4 | Balance trigger voltage | uint32 | 0.001 | V | - |
|            30-33 |      4 | SOC 100% voltage | uint32 | 0.001 | V | - |
|            34-37 |      4 | SOC 0% voltage | uint32 | 0.001 | V | - |
|            38-41 |      4 | Cell request charge voltage (RCV) | uint32 | 0.001 | V | - |
|            42-45 |      4 | Cell request float voltage (RFV) | uint32 | 0.001 | V | - |
|            46-49 |      4 | Power off voltage | uint32 | 0.001 | V | - |
|            50-53 |      4 | Max charge current | uint32 | 0.001 | A | - |
|            54-57 |      4 | Charge OCP delay | uint32 | 1.0 | s | - |
|            58-61 |      4 | Charge OCP recovery time | uint32 | 1.0 | s | - |
|            62-65 |      4 | Max discharge current | uint32 | 0.001 | A | - |
|            66-69 |      4 | Discharge OCP delay | uint32 | 1.0 | s | - |
|            70-73 |      4 | Discharge OCP recovery time | uint32 | 1.0 | s | - |
|            74-77 |      4 | Short circuit protection recovery time | uint32 | 1.0 | s | - |
|            78-81 |      4 | Max balance current | uint32 | 0.001 | A | - |
|            82-85 |      4 | Charge OTP | uint32 | 0.1 | °C | - |
|            86-89 |      4 | Charge OTP recovery | uint32 | 0.1 | °C | - |
|            90-93 |      4 | Discharge OTP | uint32 | 0.1 | °C | - |
|            94-97 |      4 | Discharge OTP recovery | uint32 | 0.1 | °C | - |
|           98-101 |      4 | Charge UTP | int32 | 0.1 | °C | - |
|          102-105 |      4 | Charge UTP recovery | int32 | 0.1 | °C | - |
|          106-109 |      4 | Power tube OTP | int32 | 0.1 | °C | - |
|          110-113 |      4 | Power tube OTP recovery | int32 | 0.1 | °C | - |
|          114-117 |      4 | Cell count | uint8 | 1.0 | - | Only first byte used |
|          118-121 |      4 | Charge switch | bool | - | - | Only first byte used |
|          122-125 |      4 | Discharge switch | bool | - | - | Only first byte used |
|          126-129 |      4 | Balancer switch | bool | - | - | Only first byte used |
|          130-133 |      4 | Nominal battery capacity | uint32 | 0.001 | Ah | - |
|          134-137 |      4 | Short circuit protection delay | uint32 | 1.0 | μs | - |
|          138-141 |      4 | Start balance voltage | uint32 | 0.001 | V | - |
|          142-269 |    128 | Connection wire resistance (1-32) | uint32 | 0.001 | Ω | 32 x 4-byte values |
|          270-273 |      4 | Device address | uint32 | - | - | Only first byte used |
|          274-277 |      4 | Precharge time | uint32 | 1 | s | Only first byte used |
|          278-281 |      4 | (Reserved) | - | - | - | - |
|          282-283 |      2 | Controls bitmask | uint16 | - | - | See bitmask notes |
|              284 |      1 | Heating start temperature | int8 | 1 | °C | - |
|              285 |      1 | Heating stop temperature | int8 | 1 | °C | - |
|              286 |      1 | Smart sleep | uint8 | - | h | - |
|              287 |      1 | Data field enable control 0 | uint8 | - | - | - |
|              299 |      1 | CRC checksum | uint8 | - | - | - |

Bitmask notes for bytes 282-283:
- bit0: Heating enabled (1)
- bit1: Disable temperature sensors (2)
- bit2: GPS Heartbeat (4)
- bit3: Port switch (1: RS485, 0: CAN) (8)
- bit4: Display always on (16)
- bit5: Special charger (32)
- bit6: Smart sleep (64)
- bit7: Disable PCL module (128)
- bit8: Timed stored data (256)
- bit9: Charging float mode (512)
- bit10-15: Reserved
