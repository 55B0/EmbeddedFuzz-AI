# Binary protocol design proposal

**Status:** Phase 0 proposal, not an implemented or finalized specification. Values below are recommended defaults for Phase 1 review.

## Scope and framing

Use one request frame per process, read from standard input until EOF. Emit one binary response on standard output. Each process starts from deterministic defaults; no state persists across test cases. Extra frames or trailing bytes are invalid rather than silently ignored. Stateful sequences are deferred.

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

## Response and rejection proposal

Use a three-byte response header: one status byte followed by an unsigned 16-bit little-endian data length, then exactly that many data bytes. Maximum response length is 67 bytes. Error responses have no data.

| Status | Meaning |
| --- | --- |
| `00` | Success |
| `01` | Invalid frame: truncated header, bad magic, payload over 64, or inconsistent total length |
| `02` | Unsupported version |
| `03` | Unsupported command |
| `04` | Invalid command payload length or sensor ID |

Recommended validation order: frame size and magic, version, command, then command payload. A well-handled request, including protocol rejection, exits normally with code zero. Crashes and diagnostics are recorded separately from protocol status. Phase 1 must specify internal I/O failure behavior.

Example proposal: request `45 46 01 01 00 00` is a valid `PING` and should return `00 00 00`. Empty input should return `01 00 00`. These are specification examples, not measured executions.

## Phase 1 review and tests

Finalize error precedence and stream behavior, then create hand-reviewed examples for all commands and errors. Test lengths around 0, 1, 63, 64, and 65; truncated headers; mismatched declarations; invalid sensor IDs; unsupported versions and commands; and trailing bytes. The reference target must be checked against these expectations, not used as its own sole oracle.

Recommend three individually enabled challenge faults: a copy-bound fault, an unchecked sensor index, and an incorrect length-validation rule. These are proposed fault families, not existing vulnerabilities. The evaluator will assign identifiers and save triggers when implemented; generator access and attribution are governed by the [research plan](research-questions.md).
