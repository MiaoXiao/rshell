# Linux Shell for CS100
## Shell Functionality

rshell is a c++ shell coded using unix. For now, there is only basic shell functionality. This includes:

* A command prompt
* Command recognition
* Connector recognition
* Comment recognition
* Exit functionality
* User and host computer recognition

Combining multiple functions is possible, of course.

## ls
After compiling, you can ls by typing:

``bin/ls``

You can also add optional parameters, such as the -a, -l, or -R flags. Currently, only the -a flag and the -l is functional.

``bin/ls -a -l``

You can specify a directory to ls too, along with any optinal flags.

``bin/ls -a src -l .``

## Kirb
Currently, there is very limited kirb functionality, which allows the user to care for a pet and see its status as you enter in commands. The command prompt will be replaced by everyones favorite cute looking puff ball.
It is as crazy as it sounds.

By typing in:

``kirb changename NAME``

You can change the default name for your virtual pet, which is Kirb.

You can also feed Kirb with the following commands:

``kirb feed peppers``

``kirb feed candy``

``kirb feed waddledee``

``kirb feed fire``

``kirb feed water``

Feeding Kirb will change how Kirb looks onscreen. To reset back to Kirbs default look, just feed him water!

##How to use
You can compile rshell and ls with:

``make``

In the root directory. This will create a bin file with the rshell and ls executable.

You can choose to specify compilation with:

``make rshell``

or:

``make ls``

To run the executable from the root directory, run:

``bin/rshell``

To clean the bin file if you want to recompile rshell or ls:

``make clean``

##Prerequisites

* C++ Compiler
* SDL Library
* Linux OS

##Bugs
Future bugs will be documented here.

* In main.cpp. adding tabs in a command might make executables that should pass, fail.
* In ls.cpp, Opendir() seems to allocate memory but does not deallocate it.

##Limitations
Limitations of rshell.

* Any command over 50,000 characters will overflow the command buffer.
* Any hostname over 300 characters will overflow the hostname buffer.
But really, you should consider having a shorter hostname in that case.
