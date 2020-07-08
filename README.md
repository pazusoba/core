# パズそば (Puzzle & Dragon solver)
I have been playing this game (the Japanese version) for 2000 days (until 2/7/2020). I started back in 2013 and it was also when I started programming and learning the Japanese language. Lots of great memories back then with my Japanese friend. 

## Goal
Really quick to find a good enough path. Ideally, it is short and cascading.

### Approaches
Currently, this is not optimal but due to the large amount of possible states, I have pruned many states but it still possible that those states will eventually lead to the optimal path. This is still under development.

#### Greedy best first search
This works but is really slow to go through all possible paths even if many are pruned. High score paths are really impressive and some of them are not something I can think of. The only problem with this approach is that often optimal path will not be found. The size of the tree is 7^step which is huge.  

#### Guided breadth first search
I am trying this approach because it will always find the optimal path. It will explore paths with the highest score first but it won't reflect the end score. I will have a list of best 100 states in the end. Hopefully, this will prevent duplicate states. 

Now, I have a queue that has a max size of 100 so the algothm will only check for the most promising states. Many states are being pruned but this is still not optimal and it hasn't really work for all boards. I will start prevent diagonal moves.

### Steps
- vertical and horizontal moves are considered as 1 step
- diagonal moves are considered as 2 - 3 steps
    - it is often harder and riskier to do diagonal moves
    - even pros might slow down and won't succeed all the time

### Board score heuristic
Now, I am thinking about scoring the board. Then, I will choose the move with the highest score.

- it is better to start from the outside (not sure why)
- vertical is better than horizontal lines by a little bit
- more board refresh means more sky fall
- similar colour in a 3x3 grid (30pt)
- 3x3 grid analysis
- two orbs in a line (100pt)
- three orbs in a line, one combo (1000pt)

There are also other options like two way, 列, 無効貫通. By giving more points, the algorithm will always try to maximise it. I can also add more pattern such as cascading. That's why the algorithm might give a more human like solution which means that it is not optimal.

## 2000 days
![2000日たまドラ](https://raw.githubusercontent.com/HenryQuan/pazusoba/master/assets/2000.jpg?token=ABTRDFH6WOWXATCBOZXXCGK7BAJ5G)

## How to compile
This is written on a windows computer so I am using the `mingw` package from `choco`. 
On Mac or Linux, you need to have `g++` installed. By tweaking the makefile, you can use commands inside.
~~~shell
$ mingw32-make.exe
$ ./a.out
~~~

Maybe I will write a GUI with qt creator but who knows.

## Resources
Things that are helpful during my experiments

- https://www.slideshare.net/mobile/tnkt37/6-37838644, tnkt37さんのスライド
    - tnkt37さん made a fancy robot that can solve the puzzle in realtime
    - He also has a [repo](https://github.com/tnkt37/PuzzDraSolver) with the code behind it
- http://maaak.net/pad/ for simulation
- https://pad.dawnglare.com/ for better simulation (support 7x6, replay, share and more)
    - https://pad.dawnglare.com/?height=6&width=7 (7x6)
- https://github.com/matthargett/padopt, an optimal PAD solver
    - This is another level and is a lot better than pazusoba
