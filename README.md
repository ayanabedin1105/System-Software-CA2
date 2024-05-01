# System Software Client-Server Assignment

This is the repository for second assignment of TUD Year 4 module - System Software

## Contributers 
- Ayan Abedin <D19125792@mytudublin.ie>

## Licence & Copyright

© Ayan Abedin, System Software 2024

## CTO Requirements

1. No direct access to the Server folders.
2. Multiple users must be able to transfer files at the same time.
3. Files transferred must be attributed to the transfer user.
4. A specific user will have access to either the (Manufacturing/Distribution) folders.
5. If synchronisation is an issue use a C Mutex Lock for Linux Thread Synchronization.

## Project Requirements

* Create a Server Socket program to run on the main Server.
* Create a Client program to connect to the server socket program.
* The system must be capable of handling multiple clients and transfers simultaneously.
* Transfer restrictions should be controlled using Real and Effective ID’s functionality.
* Create 3 user accounts and link these to three groups (Manufacturing / Distribution)
* The files transferred should be attributed to the transfer user. The file transferred to the server should show the transfer owner as its
owner.
* The client must take a file name and path via console and transfer this to the server to be stored. The following directories are where
files can be transferred to:
• Manufacturing
• Distribution
These folders will be associated with the groups described in requirement (e).
* The server must inform the client if the transfer was successful or not.

## General Assumptions.

1. The solution for the problem stated above can be developed on a single machine.
2. The client will only transfer one file at a time.


## Video Demo

* https://www.youtube.com/watch?v=vuaQY7NU3rY