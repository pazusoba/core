# パズそば (Puzzle & Dragons solver)
Puzzle & Dragons is a mobile game developed by Gungho. In this game, there is a board and you can erase orbs to make combos and damage dungeon monsters. Every combo will increase your attack by 25%. Also, there are skyfall orbs that might potentially make more combos.

## The goal
There are 30 * 3 ^ 25 possible states for a 6 x 5 board (with max steps of 25) so it is impossible to find the true optimal path. Therefore, the goal is to find a good path quickly. Ideally, it should be short, cascading and aiming for the max combo (except that it is never that ideal). 

## My approach
A special priority queue is used which limits the size to a fixed number manually. Only states with a better score can be inserted to the queue. Thus, this is a greedy approach. I have improved the search with `BEAM SEARCH` so that more states will be visited. Overall, it has surpassed me especially in two/three colour boards.

### Why beam search
Greedy DFS -> Greedy B(best)FS -> My special greedy BFS -> Beam Search

As you can see, they are all greedy algorithms based on a heuristic. The reason is that the end goal is unknown and there are also negative values. Simply choosing the local maxima may result in poor solutions. 

Best first search improves the overall performance but it consumes too much memory and is extremely slow in computation. That's why I wanted to prune useless branches because a better path is guaranteed to have a better score.

My special BFS was thus introduced. It can be seen as my attempt on the Beam Search algorithm but it has a problem, shortsighted. With size 1000, it can only check 6 steps (3^6) and after that, it only inserts if a state is better than the current best state. However, states with more potential might be blocked by local maxima because currently, it has less score.

Introducing beam search, it checks more states compared to my special BFS and accepts states with a lower score. For a beam size of 1000, it always checks for 3000 states per step and chooses the best 1000 and continue with the next step. It is not optimal but often, really close. This algorithm makes the complexity from 30 * 3 ^ 25 to 25 * 1000 * 3 (step 25, size 1000 and no diagonal moves). 

Now, with compiler optimisation and multi-threading, it runs quite fast. On my main desktop, it is even faster than padopt due to multi-threading. I am cetain that with a better `eraseOrb()` function, pazusoba can be even faster.

### Improvements
Currently, evaluating score and board related parts can be slow. By improving them, a larger size can be used to produce even better solutions.

- [x] Better heuristic (it is pretty good)
- [ ] Support different profiles
    - For example, one row, more combo, colour focused, L, + and less than certain orbs left with certain combos. 
    - Void damage penetration is kinda hard so not included
    - This can be done by updating the rate board function and the reward score
- [ ] Improving combo counting (especially for combos with many orbs sticking together)
    - Union find
    - Flood fill
- [x] Better scoring algorithm (I think the current one is good enough though)
    - Combo
    - Cascade
    - Increase the speed
- [x] Using beam search to scan the entire board (it only chooses the top ones)
    - This cuts down so many branches but at the same time, it doesn't ignore those with potential
    - My solution is now actually getting close to padopt
- [x] Compiler optimisation
- [x] Threading

## How to compile
This is written on a windows computer so I am using the `mingw` package from `choco`. 
On Mac or Linux, you need to have `g++` installed and change `win` to `mac` in the Makefile. Also, there might be some issues on mac.
~~~shell
$ mingw32-make.exe
$ ./a.exe
~~~
~~~shell
$ make
$ ./a.out
~~~
The program accepts 4 arguments
- Path to the board
- Minimum erase condition (by default 3)
- Max step (by default 25)
- Max beam size (by default 1000)

By increasing the beam size, it will take a bit more time to compute. The complexity is size * 3 * step. For size 1000 and step 25, it is 75,000.

### QT
This is my first ever qt application. I will add more functions to read boards in multiple ways and solve it at realtime to display the top results with basic replay features. However, I have to increase the speed first or it is super slow.

## Resources
Things that were helpful during my experiments

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
- flood fill & union find
- beam search

## License
My code is under [MIT license](https://github.com/HenryQuan/pazusoba/blob/master/LICENSE) but the GUI is built on top of QT so it is under [LGPL license](https://doc.qt.io/qt-5/lgpl.html) (everything inside `gui`)

# Miscellaneous
## 2000 days
I have been playing this game (the Japanese version) for more than 2000 days (until 2/7/2020). I started playing in 2013 and it was also when I started programming and learning the Japanese language. Lots of great memories back then with my Japanese friend. C++ reminds me of my good old days with C programming. You feel like you can anything with it. C is special because it was my first programming language but it was a tough way to start programming, lol. Lately, I have been using JS, Python, Dart, Swift and Kotlin. They are modern, nice and easier to write but it is nice to stop and go back to the origin once a while. 
![2000日たまドラ](https://raw.githubusercontent.com/HenryQuan/pazusoba/master/assets/2000.jpg?token=ABTRDFH6WOWXATCBOZXXCGK7BAJ5G)
たま~ たま~
