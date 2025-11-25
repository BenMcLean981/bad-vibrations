import struct
import threading
import matplotlib
import serial

import src.config as config
from src.data import collect_sample
from src.lock import STOP_EVENT
from src.plot import setup_plot

matplotlib.use("TkAgg")


def data_collection_thread(ser: serial.Serial) -> None:
    """Thread for collecting data from the serial port."""
    try:
        while not STOP_EVENT.is_set():
            try:
                collect_sample(ser)
            except serial.SerialException as e:
                print(f"Serial error: {e}")
                STOP_EVENT.set()  # Signal the thread to stop
    except KeyboardInterrupt:
        print("Data collection stopped.")


def main():
    ser = serial.Serial(config.PORT, config.BAUD_RATE, timeout=1)

    thread = threading.Thread(target=data_collection_thread, args=(ser,), daemon=True)
    thread.start()

    try:
        setup_plot()
    except KeyboardInterrupt:
        print("Serial monitor stopped.")
    finally:
        STOP_EVENT.set()  # Signal the thread to stop
        thread.join()  # Wait for the thread to finish
        ser.close()  # Close the serial port
        print("Serial port closed.")


if __name__ == "__main__":
    main()
