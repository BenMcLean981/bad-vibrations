import math
import time
from typing import List

import serial

from src.lock import LOCK
from src.sample import Sample


SAMPLES: List[Sample] = []


def collect_sample(ser: serial.Serial) -> None:
    sample = _read_sample(ser)
    with LOCK:  # Ensure thread-safe access to SAMPLES
        _try_pushing_sample(sample)


def _read_sample(ser: serial.Serial) -> Sample:
    line = ser.readline().decode("ascii").strip()  # Read a line

    segments = line.split(",")

    timestamp_ms = int(segments[0])
    acc_x = float(segments[1])
    acc_y = float(segments[2])
    acc_z = float(segments[3])

    acc = math.sqrt(acc_x**2 + acc_y**2 + acc_z**2)

    return Sample(timestamp_ms, acc)


def _try_pushing_sample(sample: Sample) -> None:
    if len(SAMPLES) == 0:
        SAMPLES.append(sample)

    last_sample = SAMPLES[len(SAMPLES) - 1]

    if sample.timestamp_ms != last_sample.timestamp_ms:
        SAMPLES.append(sample)
