# パズそば (Puzzle & Dragons solver)

## The goal
The goal is to find a good enough path quickly. Ideally, it should be short, cascading and aiming for max combo (except that it is never that ideal). 

## My approach
A speicial priority queue is used which limits the size to a fixed number. Only states with a better score than the best can be inserted to the queue. Thus, this is a greedy approch. Overall, it is better than me but it struggles to do 8 combo. **Btw, now it always start from location (2, 0) and it might not work for all boards.**

## How to compile
This is written on a windows computer so I am using the `mingw` package from `choco`. 
On Mac or Linux, you need to have `g++` installed and change `win` to `mac` in the Makefile.
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
- Max step (by default 20)
- Max queue size (by default 1000)

By increasing the queue size, it will take significantly more time to compute.

### QT
This is my first ever qt application. I will add more functions to read boards in multiple ways and solve it at realtime to display the top results with basic replay features. It might be used for Q learning in the future.

## Resources
Things that are helpful during my experiments

- https://www.slideshare.net/mobile/tnkt37/6-37838644, tnkt37さんのスライド
    - tnkt37さん made a fancy robot that can solve the puzzle in realtime
    - He also has a [repo](https://github.com/tnkt37/PuzzDraSolver) with the code behind it
- http://maaak.net/pad/ for simulation
- https://pad.dawnglare.com/ for better simulation (support 7x6, replay, share and more)
    - https://pad.dawnglare.com/?height=6&width=7 (7x6)
- https://padopt.macboy.me/, an optimal PAD solver
    - This is another level and is a lot better than pazusoba
    - https://github.com/matthargett/padopt
- https://github.com/alexknutson/Combo.Tips
- https://github.com/ethanlu/pazudora-solver
- https://puzzleanddragonsforum.com/threads/genetic-algorithm-for-pad.95084/, a great discussion
- https://cory.li/puzzle-and-dragons/, a great post
- flood fill algorithm

## License
My code is under [MIT license](https://github.com/HenryQuan/pazusoba/blob/master/LICENSE) but the GUI is built on top of QT so it is under [LGPL license](https://doc.qt.io/qt-5/lgpl.html)

## 2000 days
I have been playing this game (the Japanese version) for more than 2000 days (until 2/7/2020). I started playing in 2013 and it was also when I started programming and learning the Japanese language. Lots of great memories back then with my Japanese friend. C++ reminds me of my good old days with C programming. You feel like you can anything with it. C is special because it was my first programming language but it was a tough way to start programming, lol. Lately, I have been using JS, Python, Dart, Swift and Kotlin. They are modern, nice and easier to write but it is nice to stop and go back to the origin once a while. 
![2000日たまドラ](https://raw.githubusercontent.com/HenryQuan/pazusoba/master/assets/2000.jpg?token=ABTRDFH6WOWXATCBOZXXCGK7BAJ5G)
たま~　たま~
