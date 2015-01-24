# Linux Shell for CS100
## Description

rshell is a c++ shell coded using unix. For now, there is only basic shell functionality. This includes:

* A command prompt
* Command recognition
* Connector recognition
* Comment recognition
* Exit functionality
* User and host computer recognition

Combining multiple functions is possible, of course.

##How to use
You can compile rshell with:

``make``

In the root directory. This will create a bin file with the rshell executable.
To run the executable from the root directory, run:

``bin/rshell``

To clean the bin file if you want to recompile rshell:

``make clean``

##Prerequisites
SDL library is required. A c++ compiler is also required.

##Bugs
Future bugs will be documented here.

* ``cd`` does not work.
* Piping is not avaliable.

##Limitations
Limitations of rshell.

* Any command over 50,000 characters will overflow the command buffer.
* Any hostname over 300 characters will overflow the hostname buffer.
But really, you should consider having a shorter hostname in that case.
