## EE 250L Lab 11 Final Project - My First IoT System

We have placed some preprocessor if-else statements and if statements in 
the Makefile to help you to compile for either `openmote-b` or `native` with 
little to no code changes. For `native` make sure to create a tap interface first:

    sudo ip tuntap add tap0 mode tap user ${USER}
    sudo ip link set tap0 up

You can use `nc` as usual to send a sleep command to your RIOT instance running 
natively. To listen for packets coming from your RIOT instance, set the Relay
Node's address to your tap0 interface's address and then try this command with 
the `-l` flag for listening.
    
    nc -6luv ${IPV6_ADDR_OF_TAP0_IFACE}%tap0 8808 
