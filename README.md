# DSE3-Network-Programming
1. Modify the given code to :
a) Send 64-bit double precision numbers in place of 32-bit
b) Each client should send double precision values between a configured range (e.g -v to +v). 
Value of v should be informed to the client by the server in the first communication to
that client.
c) Make the server a multi-client one. The server should remember
the last 5 values reported by each client - and compute a running 
average for each client with last 5 reported values.
d) If the average value for any client goes above 0.75*v, the server should archive those
instances for each client-id.
e) Exchange the following structure from client to server :
    typedef struct {
        unsigned char id;
        unsigned int numElmt;
        double val[25];
    } myMsg_t;
