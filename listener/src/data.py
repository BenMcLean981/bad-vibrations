import math
import struct
from typing import List

import serial

from src.lock import LOCK
from src.sample import Sample

line_idx = -1

SAMPLES: List[Sample] = []


def collect_sample(ser: serial.Serial) -> None:
    sample = _read_sample(ser)
    with LOCK:  # Ensure thread-safe access to SAMPLES
        _try_pushing_sample(sample)


def _read_sample(ser: serial.Serial) -> Sample:
    global line_idx

    while True:
        byte = ser.read(1)
        if byte != b"\xaa":
            continue  # skip until header found

        data = ser.read(16)
        timestamp_ms, acc_x, acc_y, acc_z = struct.unpack("<Ifff", data)

        acc = math.sqrt(acc_x**2 + acc_y**2 + acc_z**2)

        return Sample(timestamp_ms, acc)


def _try_pushing_sample(sample: Sample) -> None:
    if len(SAMPLES) == 0:
        SAMPLES.append(sample)

    last_sample = SAMPLES[len(SAMPLES) - 1]

    if sample.timestamp_ms != last_sample.timestamp_ms:
        SAMPLES.append(sample)
