### EECS 338: Intro to Operating Systems and Concurrent Programming
## Assignment 5
## Author: Justin Plasmeier | jgp45@case.edu
------

# System Requirements

- Recent edition of Linux
- gcc  

# Instructions

- Clone this repository
- cd 'eecs338as5'
- make
- run 'main'

# Notes 

The code is written so that a writer thread is created first (to demonstrate the case when a writer is first to arrive), followed by a loop of random creation of readers and writers. To test the program with more readers and writers, simply change the constants defined in the main.h file, though the writers will run out of messages and write "null" instead. The program will terminate when all readers and writers are finished. 
