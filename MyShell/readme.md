
<!-- https://github.com/alanskye/CSE4100-System_Programming/tree/main/Myshell -->

# CSE4100-System-Programmnig Project 4

## MyShell

myshell simulates unix shell with system calls and signal handling. 
It can run jobs with pipe ``|`` and background ``&`` options

| directory      |                  |
| -------------- |:----------------:|
| Makefile       | project Makefile |
| src/           | source codes     |
| build/         | build directory  |
| examples/      | test programs    |
| examples/loop  | infinite loop    |
| examples/sleep | sleep for 120s   |
  
## How to Build

``$ make`` compiles \*.c under ./build/ and generates an executable file <em>myshell</em>.  

``$ make clean`` removes <em>myshell</em> and remove ./build

## References
* [recursive descent parser technique](https://github.com/Swoorup/mysh)
* Linux & Unix man pages
* CSE4100 lecture notes
