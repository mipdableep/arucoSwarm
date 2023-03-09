import cv2
import numpy as np
from TelloObject import TelloObject
from ArucoTools import ArucoTools
import threading

class SwarmControl:

    def __init__(self, leader:TelloObject, drones : list) -> None:
        """
        constructor that takes in all drones at once

        Args:
            leader (TelloObject): leader role tello
            drones (list[tello]): list of tello drone objects
        """
        self._leader = leader
        self._drones = drones
        self._allDrones = drones.append(leader)
        
    def __init__(self, drones : list) -> None:
        self._drones = drones
        self._allDrones = drones

    def do_for_all_in_threads(self, method_str:str):
        threads = []
        for tello in self._allDrones:
            method = getattr(tello, method_str)
            thread = threading.Thread(target=method)
            threads.append(thread)
            thread.start()

        # Wait for all threads to finish
        for thread in threads:
            thread.join()
            
    def do_for_all(self, method_str:str):
        for T in self._allDrones:

            parts = method_str.split('(')
            method_name = parts[0]

            method = getattr(T, method_name)
            method()