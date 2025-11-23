from matplotlib import pyplot as plt
from matplotlib.animation import FuncAnimation
from src.lock import LOCK
from src.data import SAMPLES


def update_plot(frame, line):
    """Update the live plot with the latest data."""
    with LOCK:  # Ensure thread-safe access to SAMPLES
        timestamps = [sample.timestamp_ms for sample in SAMPLES]
        distances = [sample.distance_cm for sample in SAMPLES]

    # Update the line data
    line.set_data(timestamps, distances)

    # Adjust the plot limits dynamically
    if timestamps:
        line.axes.set_xlim(
            max(0, timestamps[-1] - 5000), timestamps[-1]
        )  # Show the last 5 seconds
    if distances:
        line.axes.set_ylim(min(distances) - 1, max(distances) + 1)

    return (line,)


def setup_plot():
    """Set up the live plot."""
    fig, ax = plt.subplots()
    ax.set_title("Live Distance Data")
    ax.set_xlabel("Timestamp (ms)")
    ax.set_ylabel("Distance (cm)")
    (line,) = ax.plot([], [], lw=2)

    ani = FuncAnimation(
        fig,
        update_plot,
        fargs=(line,),
        interval=100,  # Update every 100 ms
        cache_frame_data=False,
    )

    plt.show()
