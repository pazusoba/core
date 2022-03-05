from pazusoba import adventureEx, Profile, ProfileName, Orb
import time
import random


def random_board() -> str:
    return "".join(random.choice(["L", "R", "G", "B", "D", "H"]) for _ in range(30))


def amen_benchmark():
    print("Running amen benchmark...")
    COUNT = 10

    goal_counter = 0
    steps = 0
    start = time.time()
    for i in range(COUNT):
        print("Test {}".format(i + 1))
        board = random_board()
        print("Board: {}".format(board))
        result = adventureEx(board, 3, 150, 10000, [
            Profile(name=ProfileName.COMBO, target=7),
            Profile(name=ProfileName.ORB_REMAINING, target=3),
        ])

        if result.goal:
            goal_counter += 1
        steps += result.step
    time_taken = time.time() - start
    print("It took {} seconds, {} seconds on average".format(
        time_taken, time_taken / COUNT))
    print("Goal rate: {}/{}".format(goal_counter, COUNT))
    print("Average steps: {}".format(steps / COUNT))


def combo_benchmark():
    print("Running combo benchmark...")
    COUNT = 100

    goal_counter = 0
    steps = 0
    start = time.time()
    for i in range(COUNT):
        board = random_board()
        result = adventureEx(board, 3, 150, 10000, [
            Profile(name=ProfileName.COMBO, threshold=50)
        ])

        if result.goal:
            goal_counter += 1
        steps += result.step

    time_taken = time.time() - start
    print("It took {} seconds, {} seconds on average".format(
        time_taken, time_taken / COUNT))
    print("Goal rate: {}/{}".format(goal_counter, COUNT))
    print("Average steps: {}".format(steps / COUNT))


def find_best_small_size_combo():
    COUNT = 20
    # generate same board
    boards = [random_board() for _ in range(COUNT)]
    for x in range(1, 11):
        size = x * 100
        goal_counter = 0
        steps = 0
        start = time.time()

        for i in range(COUNT):
            result = adventureEx(boards[i], 3, 150, 1000, [
                Profile(name=ProfileName.COMBO, threshold=20)
            ])

            if result.goal:
                goal_counter += 1
            steps += result.step

        time_taken = time.time() - start
        print("Size {} - avg {} s, {}/{}, avg {} steps".format(
            size, time_taken / COUNT, goal_counter, COUNT, steps / COUNT))


if __name__ == '__main__':
    print("Running benchmark")
    # amen_benchmark()
    combo_benchmark()
    # find_best_small_size_combo()
