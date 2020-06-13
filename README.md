# Multithreaded-fileserver-json
## Client side libraries - ljson-c
## Server side libraries - ljson-c, l-pthread
## Steps to execute the code
### Terminal 1(server) - gcc server.c -lpthread -ljson-c -o server
### Terminal 2(client) - gcc client.c -ljson-c -o client
### Terminal 1(server) - ./server 8888
### Terminal 2(client) - ./client localhost 8888 kr
