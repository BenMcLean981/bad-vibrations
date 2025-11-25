from matplotlib import pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np

from scipy.signal import find_peaks

from src.lock import LOCK
from src.data import SAMPLES

WINDOW_SIZE = 512  # Number of samples used for FFT
MAX_PEAKS = 10  # Number of peaks to annotate

FPS = 30


def detect_peaks(freqs, mags):
    """Return (peak_freqs, peak_mags) for the FFT spectrum."""
    if len(freqs) < 4:
        return [], []

    peak_idxs, _ = find_peaks(mags, distance=5)

    # Sort peaks by magnitude (descending), take strongest
    peak_idxs = sorted(peak_idxs, key=lambda i: mags[i], reverse=True)
    peak_idxs = peak_idxs[:MAX_PEAKS]

    return freqs[peak_idxs], mags[peak_idxs]


def compute_fft(timestamps, acc):
    """Compute FFT magnitudes and frequencies."""
    if len(acc) < 4:
        return np.array([]), np.array([])

    # Ensure the number of samples is a power of 2 (e.g., 2048)
    slice_size = 2048
    acc = np.array(acc[-slice_size:])
    timestamps = np.array(timestamps[-slice_size:])

    # Apply a Hamming window to the signal
    window = np.hamming(len(acc))
    acc = acc * window

    dt = np.diff(timestamps) / 1000.0
    avg_dt = np.mean(dt)
    if avg_dt <= 0:
        return np.array([]), np.array([])

    fft_vals = np.fft.rfft(acc)
    fft_freqs = np.fft.rfftfreq(len(acc), d=avg_dt)
    fft_mag = np.abs(fft_vals)

    fft_freqs = fft_freqs[16:]
    fft_mag = fft_mag[16:]

    return fft_freqs, fft_mag


def update_plot(frame, acc_line, fft_line, fft_ax, peak_texts):
    """Update both live plots."""

    with LOCK:
        timestamps = [s.timestamp_ms for s in SAMPLES]
        acc = [s.acc_mag for s in SAMPLES]

    # --- Time domain plot ---
    acc_line.set
    acc_line.set_data(timestamps, acc)

    if timestamps:
        acc_line.axes.set_xlim(max(0, timestamps[-1] - 5000), timestamps[-1])
    if acc:
        acc_line.axes.set_ylim(min(acc) - 1, max(acc) + 1)

    # --- FFT domain plot ---
    freqs, mags = compute_fft(timestamps, acc)
    fft_line.set_data(freqs, mags)

    if len(freqs) > 0:
        fft_ax.set_xlim(0, freqs[-1])
        fft_ax.set_ylim(0, max(max(mags) * 1.1, 5))

    # --- Peak labeling ---
    for txt in peak_texts:
        txt.remove()
    peak_texts.clear()

    peak_freqs, peak_mags = detect_peaks(freqs, mags)
    for f, m in zip(peak_freqs, peak_mags):
        txt = fft_ax.text(
            f, m, f"{f:.1f} Hz", color="blue", fontsize=9, ha="center", va="bottom"
        )
        peak_texts.append(txt)

    return acc_line, fft_line, *peak_texts


def setup_plot():
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(16, 9))

    # --- Time-domain plot ---
    ax1.set_title("Live Acceleration Data")
    ax1.set_xlabel("Timestamp (ms)")
    ax1.set_ylabel("Acceleration (g)")
    ax1.grid(True)
    (acc_line,) = ax1.plot([], [], lw=1, color="black")

    # --- FFT plot ---
    ax2.set_title("FFT Magnitude")
    ax2.set_xlabel("Frequency (Hz)")
    ax2.set_ylabel("Magnitude")
    ax2.grid(True)
    (fft_line,) = ax2.plot([], [], lw=1, color="red")

    peak_texts = []

    _ani = FuncAnimation(
        fig,
        update_plot,
        fargs=(acc_line, fft_line, ax2, peak_texts),
        interval=(1 / FPS),
        cache_frame_data=False,
    )

    plt.tight_layout()
    plt.show()
