# Linux Shell for CS100
## Shell Functionality

rshell is a c++ shell coded using unix. For now, there is only basic shell functionality. This includes:

* A command prompt
* Command recognition
* Connector recognition
* Comment recognition
* Exit functionality
* User and host computer recognition
* Input and output redirection. No piping yet!

Combining multiple functions is possible, of course.

## ls
After compiling, you can ls by typing:

``bin/ls``

in the root directoy. You can also add optional parameters, such as the -a, -l, or -R flags. 

``bin/ls -a -l -R``

You can specify a directory to ls too, along with any optional flags.

``bin/ls -a src -R . -l``

## Kirb
Currently, there is some extra shell functionality, which allows the user to care for a pet and see its status as you enter in commands. 
The command prompt will be replaced by a virtual avatar. It's as ridiculous as it sounds.

By typing in:

``kirb changename NAME``

You can change the default name for your virtual pet, which is Kirb.

You can also feed Kirb with the following commands:

``kirb feed peppers``

``kirb feed candy``

``kirb feed waddledee``

``kirb feed fire``

``kirb feed water``

Feeding Kirb will change how Kirb looks onscreen. To reset back to Kirbs default look, just feed him water.

##How to use
You can compile rshell and ls with:

``make``

In the root directory. This will create a bin file with the rshell and ls executable.

You can choose to specify compilation with:

``make rshell``

or:

``make ls``

To run the executable from the root directory:

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

##Limitations
Limitations of rshell.

* Any command over 50,000 characters will overflow the command buffer.
* Any hostname over 300 characters will overflow the hostname buffer.
But really, you should consider having a shorter hostname in that case.

Limitations of ls.

* After an invalid path is given, ls will not continue with any path or flag afterwards.
