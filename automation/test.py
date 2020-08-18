"""
Test if the pazusoba program can be called correctly
"""
import subprocess, os

completed = False
# make sure a solution is written to the disk
while not completed:
    pazusoba = subprocess.Popen(['pazusoba.exe', 'RHBDDRRGHDGBHGBGHHRLLRGBBHHRLL'], shell=True)
    pazusoba.wait()

    output_file = "path.pazusoba"
    if os.path.exists(output_file):
        solution = []
        with open(output_file, 'r') as p:
            solution = p.read().split('|')[:-1]
        solution = list(map(lambda x: x.split(','), solution))
        print(solution)
        completed = True
