# パズそば🍜
Puzzle & Dragons is a mobile game developed by Gungho. In this game, there is a board and you can erase orbs to make combos and damage dungeon monsters. Every combo will increase your attack by 25%. Also, there are skyfall orbs that might potentially make more combos.

Some demo on [YouTube](https://www.youtube.com/playlist?list=PLl6hdCL2pzdV69Ilh8vUrP2o6t2GIimqU) mostly in Japanese.

## The goal
There are 30 * 3 ^ 25 possible states for a 6 x 5 board (with max steps of 25) so it is impossible to find the true optimal path. Therefore, the goal is to find a good path quickly. Ideally, it should be short, cascading and aiming for the max combo (except that it is never that ideal).

## My approach
A priority queue is used which limits the size to a fixed number and only states with a better score can be inserted to the queue. Thus, this is a very greedy approach and it is callled `BEAM SEARCH`. Overall, it has surpassed many pro players but it is not perfect. I will keep making it better over times.

<details>
<summary>Show more</summary>

### Why beam search
Greedy DFS -> Greedy B(best)FS -> My special greedy BFS -> Beam Search

As you can see, they are all greedy algorithms based on a heuristic. The reason is that the end goal is unknown and there are also negative values. Simply choosing the local maxima may result in poor solutions.

Best first search improves the overall performance but it consumes too much memory and is extremely slow in computation. That's why I wanted to prune useless branches because a better path is guaranteed to have a better score.

My special BFS was thus introduced. It can be seen as my attempt on the Beam Search algorithm but it has a problem, shortsighted. With size 1000, it can only check 6 steps (3^6) and after that, it only inserts if a state is better than the current best state. However, states with more potential might be blocked by local maxima because currently, it has less score.

Introducing beam search, it checks more states compared to my special BFS and accepts states with a lower score. For a beam size of 1000, it always checks for 3000 states per step and chooses the best 1000 and continue with the next step. It is not optimal but often, really close. This algorithm makes the complexity from 30 * 3 ^ 25 to 25 * 1000 * 3 (step 25, size 1000 and no diagonal moves).

Now, with compiler optimisation and multi-threading, it runs quite fast. On my main desktop, it is even faster than padopt due to multi-threading. I am certain that with a better `eraseOrb()` function, pazusoba can be even faster.
</details>

### Improvements
- Safe thread
- Better heuristic for `OrbProfile` and `VoidPenProfile`
- Prevent a cycle especially when the step is more than 60
- Better queue and loop

Many improvements have been done so far. Thread is causing some issues and some profiles can be better (they are worse than me and that's not acceptable). There are many duplicate board what should I do about it? If size 20000 only takes about 3s or less, it would be amazing.

## Profiles
There are many playstyles in Puzzle & Dragons and profile is just for that. Now, it supports combo, colour, shape and orb based profiles.
- Combo focuses on doing more combo with cascading and skyfall (this is the best so far)
- Colour focuses on erasing more kinds of orbs (ideally, it should have a weight)
- Shape encourages a certain shape (2U, +, L, one row, void damage pen)
- Orb encourages to have less orbs remaining (this one doesn't work that well)

You can mix everything together and use for many teams.

<details>
<summary>Show all profiles</summary>

- ComboProfile
- ColourProfile
- ShapeProfile
    - TwoWayProfile
    - LProfile
    - PlusProfile
    - VoidPenProfile (doesn't work at the moment)
    - SoybeanProfile
    - OneRowProfile
- OrbProfile (not good enough)

*It is quite simple to add more profiles so feel free to fork this repo and add even more profiles.*
</details>

## How to compile
This is written on Windows 10 and Mac OS. On Windows, `mingw` is used to compile and use makefiles. On Mac, you just need to have xcode command line tools.

The program accepts 4 arguments
- Path to the board
- Minimum erase condition (by default 3)
- Max step (by default 50)
- Max beam size (by default 5000)

~~~shell
$ ./a.out GLHLGGLBDHDDDHGHHRDRLDDLGLDDRG 3 25 1000
$ ./a.out assets/sample_board_65.txt 3
~~~

By increasing the beam size, it will take more time (linear space) to compute. With more CPU cores, it runs significantly faster.

## Benchmark
Binaries are compiled locally and overall time are used based on the same board, max step 50 and beam size 5000. This might not be accurate. Use it as a reference.
| Version | A12Z Bionic | i5-9400 |         Note          |
| :-----: | :---------: | :-----: | :-------------------: |
|  0.1α   |   213.54s   | 110.34s |   Proof of Concept    |
|  0.2α   |   92.46s    | 39.97s  |  General Improvement  |
|  0.3α   |   12.06s    | 10.51s  | Compiler Optimisation |
|  0.4α   |    2.79s    |  2.15s  |    Multi-Threading    |
|  0.5α   |    3.06s    |  1.79s  |   Profile & OpenCV    |
|  0.6β   |    3.35s    |  2.04s  |      Automation       |
| 0.7.1β  |    1.71s    |  0.91s  |  General Improvement  |

### QT (Deprecated)
This is now replaced with `automation`. 

## Resources
Things that were helpful during my experiments.

- https://www.slideshare.net/mobile/tnkt37/6-37838644, tnkt37さんのスライド
    - tnkt37さん made a fancy robot that can solve the board in realtime
    - He also has a [repo](https://github.com/tnkt37/PuzzDraSolver) with the code behind it
    - The introduction of `beam search` really inspired me and it took me less than 5 min to implement it but it had improved the performance significantly
- http://maaak.net/pad/ for basic simulation
- https://pad.dawnglare.com/ for better simulation (support 7x6, replay, share and more)
    - https://pad.dawnglare.com/?height=6&width=7 (7x6)
- https://padopt.macboy.me/, an optimal PAD solver
    - This is an amazing optimizer
    - https://github.com/matthargett/padopt
- https://github.com/alexknutson/Combo.Tips
- https://github.com/ethanlu/pazudora-solver
- https://puzzleanddragonsforum.com/threads/genetic-algorithm-for-pad.95084/, a great discussion
- https://cory.li/puzzle-and-dragons/, a great post
- https://github.com/koduma/puzzdra_solver by kodumaさん
- flood fill & union find
- beam search

`tnkt37さん`'s video really inspired me and it was the reason why I started this project.

## License
- [MIT License](https://github.com/HenryQuan/pazusoba/blob/master/LICENSE) for pazusoba
- [LGPL License](https://doc.qt.io/qt-5/lgpl.html) for QT
- [BSD License](https://opencv.org/license/) for opencv
- [BSD License](https://github.com/asweigart/pyautogui/blob/master/LICENSE.txt) for pyautogui
- [NumPy License](https://numpy.org/devdocs/license.html)

# Miscellaneous
## 2000 days
I have been playing this game (the Japanese version) for more than 2000 days (until 2/7/2020). I started playing in 2013 and it was also when I started programming and learning the Japanese language. Lots of great memories back then with my Japanese friend. C++ reminds me of my good old days with C programming. You feel like you can do anything with it. C is special because it was my first programming language but it was a tough way to start programming, lol. Lately, I have been using JS, Python, Dart, Swift and Kotlin. They are modern, nice and easier to write but it is nice to stop and go back to the origin once a while. 
![2000日たまドラ](https://raw.githubusercontent.com/HenryQuan/pazusoba/master/assets/2000.jpg?token=ABTRDFH6WOWXATCBOZXXCGK7BAJ5G)
たま~ たま~
