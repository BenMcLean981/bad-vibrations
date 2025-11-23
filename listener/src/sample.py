from dataclasses import dataclass


@dataclass
class Sample:
    timestamp_ms: int
    distance_cm: float
