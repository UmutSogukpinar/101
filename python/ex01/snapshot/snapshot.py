import argparse
import json
import os
import time

import psutil


class SystemMonitor:
    def __init__(self, interval, output_file, snapshot_count):
        self.interval = interval
        self.output_file = output_file
        self.snapshot_count = snapshot_count

    def get_snapshot(self):

        tasks = {"total": 0, "running": 0, "sleeping": 0, "stopped": 0, "zombie": 0}
        for proc in psutil.process_iter():
            try:
                status = proc.status()
                tasks["total"] += 1
                if status in tasks:
                    tasks[status] += 1
            except (psutil.NoSuchProcess, psutil.AccessDenied):
                continue

        cpu = psutil.cpu_times_percent()
        mem = psutil.virtual_memory()
        swap = psutil.swap_memory()

        snapshot = {
            "Tasks": tasks,
            "%CPU": {
                "user": cpu.user,
                "system": cpu.system,
                "idle": cpu.idle
            },
            "KiB Mem": {
                "total": mem.total // 1024,
                "free": mem.free // 1024,
                "used": mem.used // 1024
            },
            "KiB Swap": {
                "total": swap.total // 1024,
                "free": swap.free // 1024,
                "used": swap.used // 1024
            },
            "Timestamp": int(time.time())
        }

        return snapshot

    def clear_output_file(self):
        with open(self.output_file, "w", encoding="utf-8") as file:
            file.write("")

    def write_snapshot_to_file(self, snapshot):
        with open(self.output_file, "a", encoding="utf-8") as file:
            json.dump(snapshot, file)
            file.write("\n")

    def print_snapshot(self, snapshot):
        os.system("clear" if os.name == "posix" else "cls")
        print(snapshot, end="\r")

    def run(self):
        self.clear_output_file()

        for index in range(self.snapshot_count):
            snapshot = self.get_snapshot()
            self.write_snapshot_to_file(snapshot)
            self.print_snapshot(snapshot)

            if (index < self.snapshot_count - 1):
                time.sleep(self.interval)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-i",
        help="Interval between snapshots in seconds",
        type=int,
        default=30
    )
    parser.add_argument(
        "-f",
        help="Output file name",
        default="snapshot.json"
    )
    parser.add_argument(
        "-n",
        help="Quantity of snapshot to output",
        type=int,
        default=20
    )

    args = parser.parse_args()

    monitor = SystemMonitor(args.i, args.f, args.n)
    monitor.run()


if __name__ == "__main__":
    main()