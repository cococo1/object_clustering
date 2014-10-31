Object Clustering
=================

A C++ program which detects and clusters objects on the image.

The program takes two images, one of the background and the other of the objects
on top of the background. It then displays the detected objects enclosed in a
rectangle of a specific color, each group of common objects having the same
color.

Note: This project also requires a set of OpenCV libraries, which are not included here. Check the makefile.
To build the program, run `make cluster`. To build the tests, run `make test`.
To clean the build, run `make clean`.
