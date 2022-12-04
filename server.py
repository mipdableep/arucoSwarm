import socket
import sys
import multiprocessing
from multiprocessing.synchronize import Event as EventType
from multiprocessing.context import Process as ProcessType


TAKEOFF_CMD = b"takeoff"


def send_takeoff(drone_name: str, s: socket.socket, event: EventType):
    event.wait()

    s.sendall(TAKEOFF_CMD)
    s.close()
    print(f"[+] Drone {drone_name} - Takeoff sent!")


def main():
    ip, port, drones_amount = sys.argv[1:4]
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind((ip, int(port)))
    s.listen(1)

    clients_processes: list[ProcessType] = []
    event = multiprocessing.Event()
    for i in range(int(drones_amount)):
        client_socket, _ = s.accept()
        drone_name = client_socket.recv(1024).decode()
        print(f"[-] Drone {drone_name} connected")
        clients_processes.append(
            multiprocessing.Process(
                target=send_takeoff, args=(drone_name, client_socket, event)
            )
        )
        clients_processes[i].start()

    event.set()

    for p in clients_processes:
        p.join()

    s.close()


if __name__ == "__main__":
    main()
