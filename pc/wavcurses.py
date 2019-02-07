import curses

def main(stdscr):
    stdscr.clear()
    draw(stdscr)
    c = stdscr.getkey()

def draw(stdscr):
    (lines,cols) = stdscr.getmaxyx()
    stdscr.addstr(0,0, " WAV Visualiser".ljust(cols-1), curses.A_REVERSE)

    

    stdscr.refresh()

curses.wrapper(main)
