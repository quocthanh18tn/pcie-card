
## PCIETOOL USERGUIDE

### Content

[ 1. Reading register](#readreg)

[ 2. Writing register](#writereg)

[ 3. Dump chip](#dumpchip)

[ 4. Reset chip](#resetchip)

[ 5. GPIO](#GPIO)

[ 6. Gain](#Gain)

[ 7. State](#State)

[ 8. LED](#LED)

[ 9. Frequency](#freq)

[ 10. Bandwidth](#bandwidth)

[11. Hardware information](#hwinfo)

[12. Selftest](#selftest)

[13. SFP information](#sfpinfo)

[14. System information](#sysinfo)

[15. CPU information](#cpu)

[16. Check version](#version)

[17. Appendix](#appendix)

- Chip ID list: [chipID](#chipID)
- Pin list:     [Pin](#pin)
- LED list:     [LED](#led)
- State list:   [State](#state)
- ID list:      [ID](#ID)
- Channel list: [Channel](#channel)
- Fields list:  [Fields](#field)

==========================================


<a name="readreg"></a>
### 1. Reading register

Use to read value of register from Chip. Format:
```
pcietool read reg [chipID] [reg]
```
Chip ID list: [chipID](#chipID)

Example: 
- To read value of register 0x07 from INA_12 chip, run command:
```
pcietool read reg 16 0x07
```

<a name="writereg"></a>
### 2. Writing register

Use to write value to register of Chip. Format:
```
pcietool write reg [chipID] [reg] [value]
```
Chip ID list: [chipID](#chipID)

Example:
- To write value 20 to register 0x07 of INA_12 chip, run command:
```
pcietool write reg 16 0x07 20
```
**Note:** Backup the value of register before writing new value.

<a name="dumpchip"></a>
### 3. Dump chip

Use to dump Chip. Format:
```
pcietool dump [chipID]
```
Chip ID list: [chipID](#chipID)

Example:
- To dump INA_12 chip, run command
```
pcietool dump 16
```

<a name="resetchip"></a>
### 4. Reset chip

Use to reset Chip. Format:
```
pcietool reset [chipID]
```
Chip ID list: [chipID](#chipID)

Example:
- To reset INA_12 chip, run command:
```
pcietool reset 16
```

<a name="GPIO"></a>
### 5. GPIO

Use to set or get state of GPIO. Format:
- Set state:
```
pcietool gpio set [pin] [state]
```
- Get state:
```
pcietool gpio get [pin]
```
Pin list: [Pin](#pin)

State list: [State](#state)

Example: 
- To set state ON for GPIO 26, run command:
```
pcietool gpio set 26 1
```
- To get state of GPIO 26, run command:
```
pcietool gpio get 26
```

<a name="Gain"></a>
### 6. Gain

Use to set or get value of a channel of ADRV9009. Format:
- Set value:
```
pcietool gain set [ID] [channel] [value]
```
- Get value:
```
pcietool gain get [ID] [channel]
```
ID list: [ID](#ID)

Channel list: [Channel](#channel)

Example: 
- To set value 30.0dBs for channel TX_CHANNEL_1 of ADRV9009_TRX12, run command:
```
pcietool gain set 28 0 30.0
```
- To get value of channel TX_CHANNEL_1 of ADRV9009_TRX12, run command:
```
pcietool gain get 28 0 
```

<a name="State"></a>
### 7. State

Use to set or get state of a channel of ADRV9009. Format:
- Set state:
```
pcietool state set [ID] [channel] [state]
```
- Get state:
```
pcietool state get [ID] [channel]
```
ID list: [ID](#ID)

Channel list: [Channel](#channel)

State list: [State](#state)

Example: 
- To set state ON for channel TX_CHANNEL_1 of ADRV9009_TRX12, run command:
```
pcietool state set 28 0 1
```
- To get state of channel TX_CHANNEL_1 of ADRV9009_TRX12, run command:
```
pcietool state get 28 0 
```

<a name="LED"></a>
### 8. LED

Use to set, get LED state or blink LED. Format:
- Set state:
```
pcietool led set [LED] [state]
```
- Get state:
```
pcietool led get [LED]
```
- Blink LED
```
pcietool led blink [LED] [frequency]
```
LED list: [LED](#led)

Example: 
- To set state ON for LED CPRI 0, run command:
```
pcietool led set 0 1
```
- To get state of LED CPRI 0, run command:
```
pcietool led get 0 
```
- To blink LED CPRI 0 with frequency 1Hz, run command:
```
pcietool led blink 0 1
```

<a name="freq"></a>
### 9. Frequency

Use to set or get value of frequency of ADRV9009. Format:
- Set value:
```
pcietool freq set [ID] [value]
```
- Get value:
```
pcietool freq get [ID]
```
ID list: [ID](#ID)

Example: 
- To set frequency value 1860000000Hz for channel TX_CHANNEL_1 of ADRV9009_TRX12, run command:
```
pcietool freq set 28 1860000000
```
- To get frequency value of channel TX_CHANNEL_1 of ADRV9009_TRX12, run command:
```
pcietool freq get 28
```

Morover, this function is used to set or get value of frequency of rru. Format:
- Set value:
```
pcietool freq set rru [value]
```
- Get value:
```
pcietool freq get rru
```

Example: 
- To set frequency value 1860000000Hz for rru, run command:
```
pcietool freq set rru 1860000000
```
- To get frequency value of rru, run command:
```
pcietool freq get rru
```

<a name="bandwidth"></a>
### 10. Bandwidth

Use to get value of bandwidth. Format:
```
pcietool bw [ID] [channel]
```
ID list: [ID](#ID)
Channel list: [Channel](#channel)

Example: 
- To set frequency value 10000000Hz for rru, run command:
```
pcietool bw 10000000
```

<a name="hwinfo"></a>
### 11. Hardware information

Use to set or get hardware information. Format:
- Set information:
```
pcietool hwinfo set [fields] [value]
```
- Get information:
```
pcietool hwinfo get
```
Fields list: [Fields](#field)

Example: 
- To set information of mac 2 with value 6a:3c:1f:3d:5e:3b, run command:
```
pcietool hwinfo set 2 6a:3c:1f:3d:5e:3b
```
- To get hardware information, run command:
```
pcietool hwinfo get
```

<a name="selftest"></a>
### 12. Selftest

Use to run rru selftest. Format:
```
pcietool selftest
```

<a name="sfpinfo"></a>
### 13. SFP information

Use to see SFP information. Format:
```
pcietool sfpinfo
```

<a name="sysinfo"></a>
### 14. System information

Use to see system information. Format:
```
pcietool sysinfo
```

<a name="cpu"></a>
### 15. CPU information

Use to see CPU information. Format:
```
pcietool cpu
```

<a name="version"></a>
### 16. Check version

Use to check version.
- librru version:
```
pcietool -librru --version
```

<a name="appendix"></a>
### 17. Appendix

<a name="chipID"></a>
- Chip ID list

| Chip name | ID |
|-----------|----|
| LMK05028 | 1 |
| HMC7044 | 2 |
| HMC7043 | 3 |
| SFP_1_VENDOR | 4 |
| SFP_1_DIAGNOSTIC | 5 |
| SFP_2_VENDOR | 6 |
| SFP_2_DIAGNOSTIC | 7 |
| SFP_3_VENDOR | 8 |
| SFP_3_DIAGNOSTIC | 9 |
| SFP_4_VENDOR | 10 |
| SFP_4_DIAGNOSTIC | 11 |
| RRUGPIO | 12 |
| IPCORE | 13 |
| ADT7420 | 14 |
| LM73 | 15 |
| INA_12 | 16 |
| INA_28V | 17 |
| INA_3V3_VCCO | 18 |
| INA_1V8_VCCAUX | 19 |
| INA_1V8_VCCO | 20 |
| INA_1V2_VMGTAVTT | 21 |
| INA_1V2_VCCO | 22 |
| INA_0V9_VMGTAVCC | 23 |
| INA_0V85_VCCPSINTLP | 24 |
| INA_0V85_VCCINT | 25 |
| INA_0V85_VCCPSINTFP | 25 |
| INA_28V_PA | 27 |
| ADRV9009_TRX12 | 28 |
| ADRV9009_TRX34 | 29 |
| ADRV9009_TRX56 | 30 |
| ADRV9009_TRX78 | 31 |
| ADRV9009_CALIB | 32 |
| PCA9551 | 33 |
| RRU_ID_PA | 34 |

<a name="pin"></a>
- Pin list

Check 8T8R pin list.

<a name="led"></a>
- LED list

| LED | Number |
|-----|--------|
| CPRI 0 | 0 |
| CPRI 1 | 1 |
| CPRI 2 | 2 |
| CPRI 3 | 3 |
| ALARM | 4 |

<a name="state"></a>
- State list

| State | Value |
|-------|-------|
| OFF | 0 |
| ON | 1 |

<a name="ID"></a>
- ID list

Check ID of ADRV9009 at [ChipID](#chipID) list.

<a name="channel"></a>
- Channel list

| Channel | Value |
|--------------|---|
| TX_CHANNEL_1 | 0 |
| TX_CHANNEL_2 | 1 |
| RX_CHANNEL_1 | 2 |
| RX_CHANNEL_2 | 3 |
| ORX_CHANNEL_1 | 4 |
| ORX_CHANNEL_2 | 5 |

<a name="field"></a>
- Fields list

| Fields | Value | Note |
|--------|-------|------|
| Serial number TRX | T |  |
| Serial number PA | P |  |
| Serial number Duplexer | D |  |
| Serial number full | n |  |
| Serial number full | R |  |
| date BCD | d | YYMMDDhhmmss |
| MAC table size | p |  |
| MAC 0 | 0 | XX:XX:XX:XX:XX:XX |
| MAC 1 | 1 | XX:XX:XX:XX:XX:XX |
| MAC 2 | 2 | XX:XX:XX:XX:XX:XX |
| MAC 3 | 3 | XX:XX:XX:XX:XX:XX |

