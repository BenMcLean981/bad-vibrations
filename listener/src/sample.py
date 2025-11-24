from dataclasses import dataclass


@dataclass
class Sample:
    timestamp_ms: int
    acc_mag: float
