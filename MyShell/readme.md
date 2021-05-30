
<!-- https://github.com/alanskye/CSE4100-System_Programming/tree/main/Myshell -->

# CSE4100-System-Programmnig Project 4

## MyShell

Simulation of unix shell with system calls and signal handlingA
It can run jobs with pipe ``|`` and background ``&`` options like  
`` examples/loop & cat src/*.c | grep -v "return" | sort -r ``  

| directory         |                           |
| ----------------- |:-------------------------:|
| Makefile          | project Makefile          |
| src/myshell.c     | main program (shell)      |
| src/token.c       | tokenize the command      |
| src/parser.c      | build a parse tree        |
| src/syntax.c      | execute the command       |
| src/process.c     | fork & exec               |
| build/            | build directory           |
| examples/         | test programs             |
| examples/loop     | infinite loop             |
| examples/sleep    | sleep for 120s            |
  
## How to Build

``$ make`` compiles \*.c under ./build/ and generates an executable file <em>myshell</em>  

``$ make clean`` removes <em>myshell</em> and remove ./build

## References
* [recursive descent parser technique](https://github.com/Swoorup/mysh)
* Linux & Unix man pages
* CSE4100 lecture notes
