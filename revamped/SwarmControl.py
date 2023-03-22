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
        self._formation = None
        self._formation_1L = {"t1":[0,0], "t2":[45, -75], "t3":[-45, -75]}
        """
               1,T         1 T 2
        L1:     |     L2:   \ /
               / \           |
              3   2          3
        """
        self._formation_2L = {"t1":[-45,0], "t2":[45, 0], "t3":[0, -75]}
        
        
    def set_target_all(self, target_id, target_size):
        """
        find the set aruco target for all drones

        Args:
            target_id (int): id of target
            target_size (int): size of target
        """
        # set new target
        for i in self._drones:
            i._arucoTool.set_target(target_id, target_size)
            
    
    def set_formation(self, formation, t1, t2, t3, tip_pos = [0, 0, 0]):
        self._formation = formation
        t1._position = [tip_pos[0] + self._formation["t1"][0], -tip_pos[2], -tip_pos[1] + self._formation["t1"][1]]
        t2._position = [tip_pos[0] + self._formation["t2"][0], -tip_pos[2], -tip_pos[1] + self._formation["t2"][1]]
        t3._position = [tip_pos[0] + self._formation["t3"][0], -tip_pos[2], -tip_pos[1] + self._formation["t3"][1]]

    def exit(self):
        self.do_for_all_in_threads("stop")
        self.do_for_all_in_threads("kill")

    def do_for_all_in_threads(self, method_str:str):
        threads = []
        for tello in self._drones:
            method = getattr(tello, method_str)
            thread = threading.Thread(target=method)
            threads.append(thread)
            thread.start()
            print("created thread")

        # Wait for all threads to finish
        for thread in threads:
            thread.join()
    
    
    def do_tello_command_for_all_in_threads(self, movement_cmd, args = ()):
        """
        sync movment commands (such as move_* or rotate)

        Args:
            movement_cmd (str): command
            amount (int): amount to execute (1-360)
        """
        threads = []
        for tello in self._drones:
            method = getattr(tello._tello, movement_cmd)
            thread = threading.Thread(target=method, args=args)
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