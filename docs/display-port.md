# JK-BMS display port

Since hardware version 10 the JK-BMS comes with a display port. The port can be used to turn the BMS on (short press = 1s) and off (long press = 3s).

## Connector

| Model          | Connector type                    |
| -------------- | --------------------------------- |
| JK-BD4A17S4P   | No display port                   |
| JK-BD6A17S6P   | 6 Pin, Micro JST 1.25mm pinch(?) or Micro JST GH 1.25mm pinch |
| JK-BD6A17S8P   | 6 Pin, Micro JST 1.25mm pinch(?) or Micro JST GH 1.25mm pinch |
| JK-BD6A20S6P   | 6 Pin, Micro JST 1.25mm pinch(?) or Micro JST GH 1.25mm pinch |
| JK-BD6A20S8P   | 6 Pin, Micro JST 1.25mm pinch(?) or Micro JST GH 1.25mm pinch |
| JK-BD6A24S6P   | 6 Pin, Micro JST 1.25mm pinch(?) or Micro JST GH 1.25mm pinch |
| JK-BD6A24S8P   | 6 Pin, Micro JST 1.25mm pinch(?) or Micro JST GH 1.25mm pinch |
| JK-B1A8S20P    | 6 Pin, Micro JST 1.25mm pinch(?) or Micro JST GH 1.25mm pinch |
| JK-B2A8S20P    | 6 Pin, Micro JST 1.25mm pinch(?) or Micro JST GH 1.25mm pinch |
| JK-BD6A20S10P  | 6 Pin, Micro JST GH 1.25mm pinch  |
| JK-BD6A24S10P  | 6 Pin, Micro JST GH 1.25mm pinch  |
| JK-B1A20S15P   | 6 Pin, Micro JST GH 1.25mm pinch  |
| JK-B1A24S15P   | 6 Pin, Micro JST GH 1.25mm pinch  |
| JK-B2A24S15P   | 6 Pin, Micro JST GH 1.25mm pinch  |
| JK-B2A24S20P   | 6 Pin, Micro JST GH 1.25mm pinch  |

## Schematics

```
# JK-BD6AxxSxP

│                 JK-BD6AxxSxP                    │
│                                                 │
│                     RS485              Display  │
└────────────────────[oooo]─────────────[oooooo]──┘
                                         ││
                                         ││
                                         │└─── Black
                                         └──── Red


# JK-B2A8S20P

│                   JK-B2A8S20P                   │
│                                                 │
│   Temp           RS485   Display    Balancer    │
└──[oooo]──[ooo]──[oooo]──[oooooo]──[oooooooooo]──┘
                               ││
                               ││
                               │└─── Red
                               └──── Black


# JK-BD6A2xS10P, JK-BxA24SxxP

│           JK-BD6A2xS10P, JK-BxA24SxxP           │
│                                                 │
│     CAN   RS485    Display      Heat            │
└────[ooo]─[oooo]───[oooooo]───[ooooooo]──────────┘
                     ││
                     ││
                     │└─── Black
                     └──── Red

```

## References

* https://de.aliexpress.com/item/1005003451100409.html
* https://diysolarforum.com/threads/heltec-jk-200a-smart-bms-with-2a-active-balance.17831/page-27#post-472301
* https://forum.drbacke.de/viewtopic.php?p=22931
