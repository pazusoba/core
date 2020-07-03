# パズそば (Puzzle & Dragon solver)
I have been playing this game (the Japanese version) for 2000 days (until 2/7/2020). I started back in 2013 and it was also when I started programming and learning the Japanese language. Lots of great memories with my friend. 

## Goal
The goal of this is to do at least 1 combo... If possible, it should be able to do the max possible combo with the shortest path. 

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
- https://github.com/matthargett/padopt, an optimal PAD solver
    - This is another level and is a lot better than pazusoba
