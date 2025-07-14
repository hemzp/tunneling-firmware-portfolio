# TBM API Documentation

This document describes the message protocol used between a Tunnel Boring Machine (TBM) and a central server. The protocol includes commands, status updates, and telemetry data.

## Message Format

Messages are transmitted as byte arrays.

### TBM to Server

```
[start_byte, ID, json_payload, end_byte]
```

### Server to TBM

```
[start_byte, ID, end_byte]
```

Server messages do not contain a payload.

## Event Types

| ID | Event        | Emitter | Receiver | Description                          |
|----|--------------|---------|----------|--------------------------------------|
| 1  | "TBM_INIT"   | TBM     | Server   | Sent when TBM powers on and connects to WiFi. Contains config data. |
| 2  | "TBM_START"  | Server  | TBM      | Instructs TBM to power on motor and pumps. |
| 3  | "TBM_STOP"   | Server  | TBM      | Instructs TBM to power off motor and pumps. |
| 4  | "TBM_ERROR"  | TBM     | Server   | Sent when TBM encounters an error.   |
| 5  | "TBM_DATA"   | TBM     | Server   | Periodic telemetry data.             |

## TBM to Server Message Schema

The json_payload contains telemetry data and sensor status.

### Top-level Fields

| Field             | Type       | Description                                |
|------------------|------------|--------------------------------------------|
| state            | string     | One of "config", "stop", "running", "error"|
| motor_temp       | Sensor     | Motor temperature                          |
| pump_temp        | Sensor     | Pump temperature                           |
| flow_in          | Sensor     | Inlet flow rate                            |
| flow_out         | Sensor     | Outlet flow rate                           |
| motor_power      | Sensor     | 1 or 0 indicating motor power state        |
| pump_power       | Sensor     | 1 or 0 indicating pump power state         |
| bentonite_power  | Sensor     | 1 or 0 indicating bentonite pump state     |
| estop_pressed    | Sensor     | 1 or 0 indicating emergency stop pressed   |
| global_time      | number     | Time since TBM_START in milliseconds       |

### Sensor Format

Each sensor-type field contains the following:

```json
{
  "active": 1,
  "value": 123,
  "timestamp": 1234
}
```

- `active`: 1 or 0 depending on whether the sensor is installed and reporting
- `value`: Sensor measurement or status
- `timestamp`: Milliseconds since "TBM_START"

## Example Payload

```json
{
  "state": "RUNNING",
  "motor_temp":     { "active": 1, "value": 30,  "timestamp": 1500 },
  "pump_temp":      { "active": 1, "value": 30,  "timestamp": 1505 },
  "flow_in":        { "active": 1, "value": 160, "timestamp": 1510 },
  "flow_out":       { "active": 1, "value": 150, "timestamp": 1515 },
  "motor_power":    { "active": 1, "value": 1,   "timestamp": 1520 },
  "pump_power":     { "active": 1, "value": 1,   "timestamp": 1525 },
  "bentonite_power":{ "active": 0, "value": 0,   "timestamp": 1530 },
  "estop_pressed":  { "active": 1, "value": 0,   "timestamp": 1535 },
  "global_time": 1540
}
```

## Notes

- `start_byte` and `end_byte` are framing bytes (e.g., 0x02, 0x03).
- `ID` is a numeric code for the event type.
- All timestamps are in milliseconds since TBM_START.
