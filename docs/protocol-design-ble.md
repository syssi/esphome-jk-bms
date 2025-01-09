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
