# DSE3-Network-Programming

In this assignment, we need to modify the given code to perform the following tasks:

## a) Send 64-bit double precision numbers in place of 32-bit

To send 64-bit double precision numbers instead of 32-bit, we need to modify the data type of the variable `num` in the client and server code from `int` to `double`. This will ensure that double-precision numbers are sent and received between the client and server.

## b) Each client should send double precision values between a configured range (e.g -v to +v). 
Value of v should be informed to the client by the server in the first communication to
that client.

To ensure that each client sends double precision values between a configured range, we need to first inform the client about the value of `v`. This can be done by sending the value of `v` from the server to the client in the first communication. The client can then generate random double precision values between `-v` and `+v` using the `rand()` function.

## c) Make the server a multi-client one. The server should remember
the last 5 values reported by each client - and compute a running 
average for each client with last 5 reported values.

To make the server a multi-client one, we need to modify the code to create a new thread for each new client that connects to the server. We also need to modify the server to remember the last 5 values reported by each client and compute a running average for each client using these values. This can be done by maintaining a data structure to store the last 5 values reported by each client, and updating this data structure whenever a new value is received from a client.

## d) If the average value for any client goes above 0.75*v, the server should archive those
instances for each client-id.

If the average value for any client goes above 0.75*v, the server should archive those instances for each client-id. To achieve this, we need to modify the server code to check the running average for each client after each new value is received. If the running average for a client exceeds 0.75*v, we should archive the last 5 values reported by that client.

## e) Exchange the following structure from client to server :
    typedef struct {
        unsigned char id;
        unsigned int numElmt;
        double val[25];
    } myMsg_t;

To exchange the above structure from the client to the server, we need to modify the client code to send this structure instead of sending a single double precision value. We also need to modify the server code to receive this structure and update the running average for the corresponding client based on the values in the `val` array.

These modifications will ensure that the client and server can exchange double precision values between a configured range, and that the server can handle multiple clients and compute a running average for each client. Additionally, the server can archive the last 5 values reported by each client if their running average exceeds a certain threshold.