# DSE3-Network-Programming

This repository contains the code modifications for DSE3 Network Programming assignment. The modifications made are as follows:

## Code Modifications
1. Modified the code to send 64-bit double precision numbers instead of 32-bit.
2. Each client sends double precision values between a configured range (e.g., -v to +v). The value of `v` is informed to the client by the server in the first communication to that client.
3. Made the server a multi-client one. The server remembers the last 5 values reported by each client and computes a running average for each client with the last 5 reported values.
4. If the average value for any client goes above 0.75*v, the server archives those instances for each client-id.
5. Exchanged the following structure from client to server:
    ```
    typedef struct {
        unsigned char id;
        unsigned int numElmt;
        double val[25];
    } myMsg_t;
    ```


## Contributors
- [Dhruba Saha](https://github.com/dhrubasaah08) - made the modifications to the code.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.


## License
This project is released under the Unlicense. See the `LICENSE` file for more information.