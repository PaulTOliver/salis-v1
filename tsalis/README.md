# TSALIS
*TSALIS* is a text user interface (TUI) designed to communicate with *SALIS*.
Its only dependencies are ncurses and the *SALIS* library itself. It should be
portable enough and should run easily in any terminal environment.

## Building instructions
You'll need nothing but a C compiler (C89). You must build the program and link
it with *SALIS* (e.g. *libsalis.a*) and ncurses. A sample makefile
(Makefile) is provided for GNU Make. You can also just run the `make` command
inside the salis directory and it will automate the building and linking of
both the library and this application. Feel free to edit both makefiles as
needed. If you run into any difficulties, please let me know!

## List of commands
### Command-line arguments
You may run *TSALIS* from the terminal in any of these three ways (arguments
are being represented by *XX*). Note that, upon exit, *SALIS* automatically
generates a save (by default called *defsim*). This save file may be freely
renamed (any name 10 characters or shorter) and reloaded as needed.

|Arguments      |Action                                                                         |
|:--------------|------------------------------------------------------------------------------:|
|tsalis         |If file *defsim* exists in directory, loads simulation from that file.         |
|tsalis         |If file *defsim* does not exist, creates new simulation (memory size 2^16).    |
|tsalis n*XX*   |Creates new simulation with memory size 2^*XX*.                                |
|tsalis l*XX*   |Loads simulation from file named *XX*.                                         |

### Keyboard commands
|Key            |Action                                                 |
|:--------------|------------------------------------------------------:|
|Left arrow     |Previous page                                          |
|Right arrow    |Next page                                              |
|wasd           |Scroll (PROCESS and WORLD page)                        |
|W              |Scroll to top (PROCESS and WORLD page)                 |
|A              |Scroll to left (PROCESS page)                          |
|zx             |Zoom in/out (WORLD page)                               |
|op             |Select previous/next organism                          |
|g              |Toggle data/gene view (PROCESS page)                   |
|c              |Open console (pauses simulation)                       |
|Space          |Run/pause simulation                                   |
|jl             |Select first/last organism                             |
|k              |Go to selected organism (PROCESS and WORLD page)       |
|Numbers (1-0)  |Cycle simulation (1 = 1, 2 = 2, 3 = 4, 4 = 8, ...)     |

### Console commands
The console opens up when 'c' is pressed. Commands, with their respective
parameters separated by underscores, may be written in order to modify or
control some aspects of the simulation. Parameters here are represented by
*XX*.

|Command     |Param. 1    |Param. 2    |Action                                                                |
|:-----------|:-----------|:-----------|---------------------------------------------------------------------:|
|q           |---         |---         |Save and quit simulation.                                             |
|i*XX*\_*XX* |address     |instructions|Writes given instructions into address.                               |
|c*XX*\_*XX* |address     |file name   |Compiles given file into address.                                     |
|n*XX*\_*XX* |address     |size        |Initializes organism of given size into address.                      |
|k           |---         |---         |Kills organism at bottom of queue (first organism).                   |
|m*XX*       |address     |---         |Scroll/move (PROCESS and WORLD page) to given process/address.        |
|p*XX*       |process id  |---         |Select given process.                                                 |
|s           |---         |---         |Save simulation.                                                      |
|r*XX*       |name        |---         |Rename simulation (will be automatically saved to this name on exit). |
|a*XX*       |interval    |---         |Set simulation's auto-save interval.                                  |
|?           |---         |---         |Fill memory with random data.                                         |

### Legend
In WORLD view, as well as in PROCESS view (when gene mode is selected), each
cell is colored according to the following legend:

|Background color |Meaning                                 |
|:----------------|---------------------------------------:|
|BLUE             |Non-allocated cell                      |
|CYAN             |Allocated cell                          |
|WHITE            |Start of memory block                   |
|YELLOW           |Main memory block of selected organism  |
|GREEN            |Child memory block of selected organism |
|MAGENTA          |SP of selected organism                 |
|RED              |IP of selected organism                 |
