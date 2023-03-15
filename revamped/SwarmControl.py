import threading

class SwarmControl:

    def __init__(self, drones : list) -> None:
        self._drones = drones

    def do_for_all_in_threads(self, method_str:str):
        threads = []
        for tello in self._drones:
            method = getattr(tello, method_str)
            thread = threading.Thread(target=method)
            threads.append(thread)
            thread.start()

        # Wait for all threads to finish
        for thread in threads:
            thread.join()
    
    def do_for_all(self, method_str:str):
        for T in self._drones:

            parts = method_str.split('(')
            method_name = parts[0]

            method = getattr(T, method_name)
            method()