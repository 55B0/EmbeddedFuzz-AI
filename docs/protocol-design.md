# Binary protocol specification

**Status:** Version 1 specification for the Phase 1 target.

## Scope and framing

Use one request frame per process. The target reads binary standard input until EOF or until it receives a 71st byte, which is enough to reject an oversized request. It emits one binary response on standard output. Each process starts from deterministic defaults. Extra frames and trailing bytes are invalid.

| Offset | Field | Proposed encoding |
| --- | --- | --- |
| 0 | Magic | Two bytes: `45 46` (ASCII `EF`) |
| 2 | Version | One byte: `01` |
| 3 | Command | One byte, defined below |
| 4 | Payload length | Unsigned 16-bit integer, little-endian |
| 6 | Payload | Exactly the declared number of bytes; at most 64 |

A valid request is at most 70 bytes. Recommend a separate harness ceiling of 256 bytes, allowing oversized protocol inputs to reach rejection paths. Empty input is an admissible test case. Inputs beyond the harness ceiling are admission rejections, not target failures. The target should also bound its own reads and reject overlong input.

Omit a checksum initially: it adds an integrity gate that can obscure parser exploration without answering the first research question. If introduced later, give all strategies access to the same repair helper, record whether repair was used, and retain a way to bypass repair for malformed tests. That would be a separately described experiment.

## Commands

| ID | Name | Request payload | Successful response data |
| --- | --- | --- | --- |
| `01` | `PING` | Empty | Empty |
| `02` | `READ_SENSOR` | One unsigned byte, sensor ID 0 or 1 | Unsigned 16-bit little-endian value: 100 for ID 0, 200 for ID 1 |
| `03` | `ECHO` | Zero to 64 arbitrary bytes | Exact payload |

These commands exercise dispatch, command-specific length checks, a bounded lookup, and variable-length copying without configuration state. Sensor values are synthetic constants, not physical readings. Sequence IDs, configuration writes, and resets are unnecessary for one-request processes and are deferred.

## Response and rejection

Use a three-byte response header: one status byte followed by an unsigned 16-bit little-endian data length, then exactly that many data bytes. Maximum response length is 67 bytes. Error responses have no data.

| Status | Meaning |
| --- | --- |
| `00` | Success |
| `01` | Invalid frame: truncated header, bad magic, payload over 64, or inconsistent total length |
| `02` | Unsupported version |
| `03` | Unsupported command |
| `04` | Invalid command payload length or sensor ID |

Validation order is frame size, magic, and declared length, then version, command, and command payload. A handled request, including a protocol rejection, exits with code zero. A detected stream setup, read, or write failure emits a diagnostic on standard error and returns exit code 1. A write failure may leave a partial response.

| Request bytes | Expected response bytes | Meaning |
| --- | --- | --- |
| `45 46 01 01 00 00` | `00 00 00` | PING |
| `45 46 01 02 01 00 00` | `00 02 00 64 00` | READ_SENSOR, sensor 0 |
| `45 46 01 03 02 00 00 FF` | `00 02 00 00 FF` | ECHO, two bytes |
| `45 46 01 02 01 00` | `01 00 00` | Missing payload |
| `45 46 02 01 00 00` | `02 00 00` | Unsupported version |
| `45 46 01 FF 00 00` | `03 00 00` | Unsupported command |
| `45 46 01 02 01 00 FF` | `04 00 00` | Invalid sensor ID |

## Conformance and challenge targets

The [conformance checks](../tests/test_target.py) compare process output with exact expected bytes and verify each challenge build against its evaluator fixture. The default build excludes all three injected faults. Each challenge build enables one semantic fault, allowing repeatable failures without relying on undefined behavior. Generator access and attribution are governed by the [research plan](research-questions.md).
