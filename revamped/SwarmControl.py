import threading
import TelloObject

class SwarmControl:

    def __init__(self, drones : list) -> None:
        self._drones = drones

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
        
        # stop all movment
        self.do_for_all_in_threads("stop")



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
    
    
    def do_movement_command_for_all_in_threads(self, movement_cmd, amount):
        """
        sync movment commands (such as move_* or rotate)

        Args:
            movement_cmd (str): command
            amount (int): amount to execute (1-360)
        """
        threads = []
        for tello in self._drones:
            method = getattr(tello._tello, movement_cmd)
            thread = threading.Thread(target=method, args=(amount))
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