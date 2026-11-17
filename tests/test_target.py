"""Phase 1 conformance checks for compiled reference and challenge targets."""

import subprocess
import sys
from pathlib import Path


def check(target, name, request, expected):
    result = subprocess.run([str(Path(target).resolve())], input=request,
                            capture_output=True, timeout=5)
    if (result.returncode, result.stdout, result.stderr) != (0, expected, b""):
        raise AssertionError(
            f"{target}: {name}: expected {expected.hex(' ')}, "
            f"got {result.stdout.hex(' ')} (exit {result.returncode}, "
            f"stderr {result.stderr!r})"
        )


def main(reference, *variants):
    ping = bytes.fromhex("45 46 01 01 00 00")
    sensor = bytes.fromhex("45 46 01 02 01 00")
    echo = bytes.fromhex("45 46 01 03")
    ok, frame_error, version_error, command_error, payload_error = (
        bytes((status, 0, 0)) for status in range(5)
    )
    payload = bytes(range(1, 65))
    cases = [
        ("ping", ping, ok),
        ("sensor zero", sensor + b"\0", bytes.fromhex("00 02 00 64 00")),
        ("sensor one", sensor + b"\1", bytes.fromhex("00 02 00 c8 00")),
        ("missing sensor byte", sensor, frame_error),
        ("invalid sensor", sensor + b"\xff", payload_error),
        ("empty sensor payload", bytes.fromhex("45 46 01 02 00 00"), payload_error),
        ("ping with payload", bytes.fromhex("45 46 01 01 01 00 ff"), payload_error),
        ("trailing byte", ping + b"\n", frame_error),
        ("bad magic", b"\0" + ping[1:], frame_error),
        ("wrong byte order", echo + b"\0\1" + b"x", frame_error),
        ("oversized frame", echo + b"\x41\0" + bytes(65), frame_error),
        ("unsupported version", bytes.fromhex("45 46 02 ff 00 00"), version_error),
        ("unsupported command", bytes.fromhex("45 46 01 ff 00 00"), command_error),
        ("frame error takes precedence", bytes.fromhex("45 46 02 ff 01 00"), frame_error),
    ]
    cases += [(f"truncated header {n}", ping[:n], frame_error) for n in range(6)]
    cases += [
        (f"echo {n} bytes", echo + bytes((n, 0)) + payload[:n],
         bytes((0, n, 0)) + payload[:n]) for n in (0, 1, 63, 64)
    ]
    for case in cases:
        check(reference, *case)

    # Evaluator fixtures. Keep these out of generator inputs and prompts.
    faults = [
        ("echo 64 bytes", bytes.fromhex("00 3f 00") + payload[:63]),
        ("invalid sensor", bytes.fromhex("00 02 00 c8 00")),
        ("trailing byte", ok),
    ]
    for target, (trigger, symptom) in zip(variants, faults):
        for name, request, expected in cases:
            check(target, name, request, symptom if name == trigger else expected)
        request = next(request for name, request, _ in cases if name == trigger)
        for _ in range(2):
            check(target, trigger, request, symptom)
    print(f"{len(cases)} reference cases passed; {len(variants)} fault variants verified.")


if __name__ == "__main__":
    if len(sys.argv) not in (2, 5):
        sys.exit("Usage: python tests/test_target.py REFERENCE [F001 F002 F003]")
    main(*sys.argv[1:])
