# Overview

This code has three part. Server part, Client part, MARS part.  Server can use MARS program on multiple boards. Network protocol used TCP. Int the board, communication between client and MARS used shared memory(IPC).



# compiling

There is a Makefile. 

- Generate Server
  - `make server`

- Generate Client
  - `make client`
- Generate MARS(Test)
  - `make mars`
- Generate Client and MARS(Test)
  - `make board`
- Generate All files
  - `make all`

If you setup in board, please type `make board`. Then they'll make client and mars program.



