def getEachOrb(image, board_size, orb_count, border_len):
    """
    Based on count, get a list of every orb, this is also an ordered list
    """

    # weight, height
    w, h = board_size
    orbs = []

    global board_column, board_row
    if orb_count == 20:
        # 5x4
        board_column = 5
        board_row = 4
    elif orb_count == 30:
        # 6x5
        board_column = 6
        board_row = 5
    elif orb_count == 42:
        # 7x6
        board_column = 7
        board_row = 6
    else:
        print("Unknown orb count.")

    # consider added padding here
    initial = border_len * 2
    orb_w = int((w + initial) / board_column)
    orb_h = int((h + initial) / board_row)
    x1 = y1 = initial
    x2 = y2 = initial
    # start getting every orb
    for _ in range(board_row):
        # update x2 and y2 first
        y2 += orb_h
        for _ in range(board_column):
            x2 += orb_w
            # NOTE: you need to save a copy here because it changes image
            orbs.append(image[y1:y2, x1:x2].copy())
            # update x1 and y2 later
            x1 += orb_w
        y1 += orb_h
        # reset x1 and x2
        x1 = x2 = initial
    return orbs