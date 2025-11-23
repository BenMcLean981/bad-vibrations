import threading

LOCK = threading.Lock()  # To synchronize access to SAMPLES

STOP_EVENT = threading.Event()  # Event to signal the thread to stop
