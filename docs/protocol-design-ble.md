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
3. Send device info command on characteristic `0xFFE1` (handle `0x03`)
4. Send cell info command if no status notification is received

## Command Structure

Commands are sent as 20-byte frames with the following format:

| Offset | Length | Description |
|--------|---------|-------------|
| 0-3 | 4 | Header (0xAA 0x55 0x90 0xEB) |
| 4 | 1 | Command byte |
| 5 | 1 | Length byte |
| 6-9 | 4 | Value (32-bit) |
| 10-18 | 9 | Zero padding |
| 19 | 1 | CRC |

### Command Types

- Device Info: `0x97`
- Cell Info: `0x96`

### Example Command Payloads

#### Device Info Command (0x97)
```
AA 55 90 EB     # Header
97              # Command
00              # Length
00 00 00 00     # Value
00 00 00 00     # Padding
00 00 00 00     
00 00 00 00
00              # CRC (calculated across bytes 0-18)
```

#### Cell Info Command (0x96)
```
AA 55 90 EB     # Header
96              # Command  
00              # Length
00 00 00 00     # Value
00 00 00 00     # Padding
00 00 00 00     
00 00 00 00
00              # CRC (calculated across bytes 0-18)
```

## Response Handling

### Frame Assembly
- Complete responses range from 300-320 bytes
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
|-----------------|--------|-------------|---------|-------------|------|-------|
| 0-3 | 4 | Header | Raw | - | - | Fixed value: 0x55 0xAA 0xEB 0x90 |
| 4 | 1 | Record type | uin8t | - | - | Fixed value: 0x02 |
| 5 | 1 | Frame counter | uint8 | - | - | - |
| 6-69 | 64 | Cell voltages 1-32 | uint16 | 0.001 | V | 2 bytes per cell |
| 54-57 | 4 | Enabled cells bitmask | Raw | - | - | 0xFF 0xFF 0xFF 0xFF = 32 cells |
| 64-127 | 64 | Cell resistances 1-32 | uint16 | 0.001 | Ohm | 2 bytes per cell |
| 112-113 | 2 | Power tube temperature | int16 | 0.1 | °C | - |
| 114-117 | 4 | Wire resistance warning bitmask | Raw | - | - | 1 bit per cell/wire |
| 118-121 | 4 | Battery voltage | uint32 | 0.001 | V | - |
| 122-125 | 4 | Battery power | uint32 | 0.001 | W | - |
| 126-129 | 4 | Charge current | int32 | 0.001 | A | - |
| 130-131 | 2 | Temperature Sensor 1 | int16 | 0.1 | °C | - |
| 132-133 | 2 | Temperature Sensor 2 | int16 | 0.1 | °C | - |
| 134-135 | 2 | Errors bitmask | uint16 | - | - | System error flags |
| 138-139 | 2 | Balance current | int16 | 0.001 | A | - |
| 140 | 1 | Balancing action | Raw | - | - | 0=Off, 1=Charging, 2=Discharging |
| 141 | 1 | State of charge | uint8 | 1 | % | - |
| 142-145 | 4 | Remaining capacity | uint32 | 0.001 | Ah | - |
| 146-149 | 4 | Nominal capacity | uint32 | 0.001 | Ah | - |
| 150-153 | 4 | Cycle count | uint32 | 1 | - | - |
| 154-157 | 4 | Total cycle capacity | uint32 | 0.001 | Ah | - |
| 158 | 1 | State of Health (SOH) | uint8 | 1 | % | - |
| 159 | 1 | Precharge status | Raw | - | - | 0=Off, 1=On |
| 162-165 | 4 | Total runtime | uint32 | 1 | s | - |
| 166 | 1 | Charging MOSFET status | Raw | - | - | 0=Off, 1=On |
| 167 | 1 | Discharging MOSFET status | Raw | - | - | 0=Off, 1=On |
| 168 | 1 | Precharging status | Raw | - | - | 0=Off, 1=On |
| 222-223 | 2 | Temperature Sensor 5 | int16 | 0.1 | °C | - |
| 224-225 | 2 | Temperature Sensor 4 | int16 | 0.1 | °C | - |
| 226-227 | 2 | Temperature Sensor 3 | int16 | 0.1 | °C | - |
| 186-187 | 2 | Emergency time countdown | uint16 | 1 | s | >0 = Emergency active |
| 299 | 1 | CRC checksum | uint8 | - | - | - |

### Device Info Frame Specification (`JK02_32S`)

| Position (Bytes) | Length | Description | Format | Coefficient | Unit | Notes |
|-----------------|--------|-------------|---------|-------------|------|-------|
| 0-3 | 4 | Frame header | hex | - | - | Fixed value: 0x55 0xAA 0xEB 0x90 |
| 4 | 1 | Frame type | uint8 | - | - | Fixed value: 0x03 |
| 5 | 1 | Frame counter | uint8 | - | - | Increments with each frame |
| 6-21 | 16 | Vendor ID | string | - | - | Null-terminated string |
| 22-29 | 8 | Hardware version | string | - | - | Null-terminated string |
| 30-37 | 8 | Software version | string | - | - | Null-terminated string |
| 38-41 | 4 | Device uptime | uint32 | 1 | seconds | Little-endian |
| 42-45 | 4 | Power on count | uint32 | 1 | count | Little-endian |
| 46-61 | 16 | Device name | string | - | - | Null-terminated string |
| 62-77 | 16 | Device passcode | string | - | - | Null-terminated string |
| 78-85 | 8 | Manufacturing date | string | - | - | Null-terminated string |
| 86-96 | 11 | Serial number | string | - | - | Null-terminated string |
| 97-101 | 5 | Passcode | string | - | - | Null-terminated string |
| 102-117 | 16 | User data | string | - | - | Null-terminated string |
| 118-133 | 16 | Setup passcode | string | - | - | Null-terminated string |
| 184 | 1 | UART1M Protocol number | uint8 | - | - | Protocol version |
| 185 | 1 | CAN Protocol number | uint8 | - | - | Protocol version |
| 218 | 1 | UART2M Protocol number | uint8 | - | - | Protocol version |
| 219 | 1 | UART2M Protocol enable | uint8 | - | - | Enable flag |
| 234 | 1 | LCD buzzer trigger | uint8 | - | - | Trigger setting |
| 235 | 1 | DRY1 Trigger | uint8 | - | - | Trigger setting |
| 236 | 1 | DRY2 Trigger | uint8 | - | - | Trigger setting |
| 237 | 1 | UART protocol library version | uint8 | - | - | Protocol version |
| 238-241 | 4 | LCD Buzzer Trigger Value | uint32 | - | - | Little-endian |
| 242-245 | 4 | LCD Buzzer Release Value | uint32 | - | - | Little-endian |
| 246-249 | 4 | DRY1 Trigger Value | uint32 | - | - | Little-endian |
| 250-253 | 4 | DRY1 Release Value | uint32 | - | - | Little-endian |
| 254-257 | 4 | DRY2 Trigger Value | uint32 | - | - | Little-endian |
| 258-261 | 4 | DRY2 Release Value | uint32 | - | - | Little-endian |
| 262-265 | 4 | Data Stored Period | uint32 | - | - | Little-endian |
| 266 | 1 | RCV Time | uint8 | 0.1 | hours | Request Charge Voltage Time |
| 267 | 1 | RFV Time | uint8 | 0.1 | hours | Request Float Voltage Time |
| 268 | 1 | CAN protocol library version | uint8 | - | - | Protocol version |
| 299 | 1 | CRC checksum | uint8 | - | - | - |
