"""
The time taken to read in a board from command line or a board file
"""
import time
import os, subprocess

# mac or win
platform="win"
if platform == "mac":
    program = "../automation/pazusoba.out"
else:
    program = "../automation/pazusoba.exe"

# update boards here
board = "RHBDDRRGHDGBHGBGHHRLLRGBBHHRLL"

print("Benchmark")
overall = time.time()
time_taken = 0
count = 10

while count > 0:
    start = time.time()
    # run the program
    pazusoba = subprocess.Popen([program, board, '3', '50', '10000'])
    pazusoba.wait()

    output_file = "path.pazusoba"
    if os.path.exists(output_file):
        count -= 1
        time_taken += time.time() - overall

print("It took %.2fs, on average %.2fs" % (time.time() - overall) % time_taken / count)
