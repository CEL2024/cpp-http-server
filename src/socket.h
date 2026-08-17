#pragma once
// starts the server on port 8080 of your local machine and sets up the server to listen for incoming connections.
int startServer();
// this function is purely for testing at this stage. Only works with one connection and sometimes doesn't disconnect after.
int test_accept(int s_fd);

