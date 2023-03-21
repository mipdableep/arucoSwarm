import threading
import TelloObject
import cv2

class SwarmControl:

    def __init__(self, drones : list) -> None:
        self._drones = drones

    def set_dist(self, right, left):
        self.rightPos = right
        self.leftPos = left

    def all_find_target(self, target_id, target_size):
        """
        find the set aruco target for all drones

        Args:
            target_id (int): id of target
            target_size (int): size of target
        """
        
        target_found_list = [False for i in self._drones]
        
        # set new target
        for i in self._drones:
            i._arucoTool.set_target(target_id, target_size)
        
        # all drones found target before moving on
        while False in target_found_list:
            for index, tello in enumerate(self._drones):
                target_found_list[index] = tello.find_target()
            if cv2.waitKey(20) == ord('q'):
                self.do_for_all_in_threads("stop")
                return False
            print(target_found_list)
        
        # stop all movment
        self.do_for_all_in_threads("stop")
        return True
    
    def go_into_formation(self, leader, right, left):        
        s, R, T = leader.get_location()
        while s == -9:
            s, R, T = leader.get_location()
        
        leader_pos = {"lr":T["lr"], "fb":T["fb"], "ud":T["ud"]}
        # TODO write th rest for formation
        ret = True
        
        while False in [leader.target_reached, right.target_reached, left.target_reached]:
            leader.trackLoop (leader_pos)
            right.trackLoop ({"lr":leader_pos["lr"] + self.rightPos[0],
                            "fb":leader_pos["fb"] + self.rightPos[1],
                            "ud":leader_pos["ud"]})
            
            left.trackLoop ({"lr":leader_pos["lr"] + self.leftPos[0],
                            "fb":leader_pos["fb"] + self.leftPos[1],
                            "ud":leader_pos["ud"]})
            if cv2.waitKey(20) != ord('q'):
                ret = False
        
        leader.target_reached, right.target_reached, left.target_reached = False, False, False
        
        return ret

    def go_to_target(self,target , leader, right, left):
        
        ret = True
        
        while False in [leader.target_reached, right.target_reached, left.target_reached]:
            s, R, T = leader.get_location()
            while s == -9:
                s, R, T = leader.get_location()
            leader_pos = {"lr":T["lr"], "fb":T["fb"], "ud":T["ud"]}
            
            bias = leader.trackLoop (target)
            right.trackLoop ({"lr":leader_pos["lr"] + self.rightPos[0],
                            "fb":leader_pos["fb"] + self.rightPos[1],
                            "ud":leader_pos["ud"]}, bias)
            
            left.trackLoop ({"lr":leader_pos["lr"] + self.leftPos[0],
                            "fb":leader_pos["fb"] + self.leftPos[1],
                            "ud":leader_pos["ud"]}, bias)
            if cv2.waitKey(20) != ord('q'):
                ret = False
        
        leader.target_reached, right.target_reached, left.target_reached = False, False, False
        
        return ret

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