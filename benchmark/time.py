"""
The time taken to read in a board from command line or a board file
"""
import time
import os, subprocess, sys

program = sys.argv[1]

# update boards here
board = "RHBDDRRGHDGBHGBGHHRLLRGBBHHRLL"

overall = time.time()
time_taken = 0
count = 10
divider = count
crash_count = 0

while count > 0:
    # run the program
    pazusoba = subprocess.Popen([program, board, '3', '50', '5000'], stdout=subprocess.DEVNULL)
    start = time.time()
    pazusoba.wait()

    output_file = "path.pazusoba"
    if os.path.exists(output_file):
        count -= 1
        curr_time = time.time() - start
        print("{}s".format(curr_time))
        time_taken += curr_time
    else:
        crash_count += 1
    time.sleep(0.2)

print("It took {}s, on average {}s. crashed {} times".format(time.time() - overall, time_taken / divider, crash_count))

# use this on windows with powershell
# Measure-Command { .\2.exe .\assets\sample_board_65.txt 3 50 5000 | Out-Default }
