import io
import math
from typing import List, Optional

from src.lock import LOCK
from src.sample import Sample


SAMPLES: List[Sample] = []

has_read = False


def collect_sample(text_stream: io.TextIOWrapper) -> None:
    sample = _read_sample(text_stream)

    if sample:
        with LOCK:
            _try_pushing_sample(sample)


def _read_sample(text_stream: io.TextIOWrapper) -> Optional[Sample]:
    global has_read

    line = text_stream.readline()

    if not line.startswith("l:"):
        if not has_read:
            return
        else:
            raise ValueError("Line format error")

    has_read = True

    segments = line[2:].split(",")

    timestamp_ms = int(segments[0])

    acc_x = float(segments[1])
    acc_y = float(segments[2])
    acc_z = float(segments[3])

    acc_mag = math.sqrt(acc_x**2 + acc_y**2 + acc_z**2)

    return Sample(timestamp_ms, acc_mag)


def _try_pushing_sample(sample: Sample) -> None:
    if len(SAMPLES) == 0:
        SAMPLES.append(sample)

    last_sample = SAMPLES[len(SAMPLES) - 1]

    if sample.timestamp_ms != last_sample.timestamp_ms:
        SAMPLES.append(sample)
