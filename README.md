Inter-process communication
===================

# Using chmod to processes and make to execute the program

This guide will show you how to use chmod and make to execute the program.

## Step 1: Set Execute Permissions

First, you need to set execute permissions on the program. This is done by using the `chmod` command with the `a+x` option.

```
chmod a+x proc_* main
```

This command will set execute permissions for all files starting with `proc_` and the `main` program.

## Step 2: Compile and Run the Program

Next, you need to compile and run the program. This is done using the `make` command.

```
make all && ./main <port1> <port2>
```

This command will compile all the source files for the program and run it with the given ports .

## Step 3: Output

The output of the program should look something like this:

```
./main 6076 9927
main.cpp::main(): pipe1r_string: 5  pipe1w_string: 6
main.cpp::start_proc(): [debug] Printing args for process proc_p1:
main.cpp::start_proc(): [0]: |proc_p1|
main.cpp::start_proc(): [1]: |6|
main.cpp::start_proc(): [2]: |5|
main.cpp::wait_for_signal_sync(): waiting for signal sync status from process proc_p1...
main.cpp::handle_sync_signal(): got a sync signal: 1
main.cpp::start_proc(): [debug] Printing args for process proc_p2:
main.cpp::start_proc(): [0]: |proc_p2|
main.cpp::start_proc(): [1]: |6|
main.cpp::start_proc(): [2]: |5|
P1: Started
P1: Ended
main.cpp::wait_for_signal_sync(): waiting for signal sync status from process proc_p2...
main.cpp::handle_sync_signal(): got a sync signal: 2
main.cpp::start_proc(): [debug] Printing args for process proc_pr:
main.cpp::start_proc(): [0]: |proc_pr|
main.cpp::start_proc(): [1]: |1420402|
main.cpp::start_proc(): [2]: |1420403|
main.cpp::start_proc(): [3]: |5|
main.cpp::start_proc(): [4]: |10|
P2: Started
P2: Ended
main.cpp::wait_for_signal_sync(): waiting for signal sync status from process proc_pr...
main.cpp::handle_sync_signal(): got a sync signal: 3
main.cpp::start_proc(): [debug] Printing args for process proc_serv2:
main.cpp::start_proc(): [0]: |proc_serv2|
main.cpp::start_proc(): [1]: |9927|
main.cpp::wait_for_signal_sync(): waiting for signal sync status from process proc_serv2...
Converted port number
Opened/created serv2.txt
Created socket
Bound socket
main.cpp::handle_sync_signal(): got a sync signal: 4
main.cpp::start_proc(): [debug] Printing args for process proc_serv1:
main.cpp::start_proc(): [0]: |proc_serv1|
main.cpp::start_proc(): [1]: |6076|
main.cpp::start_proc(): [2]: |9927|
main.cpp::wait_for_signal_sync(): waiting for signal sync status from process proc_serv1...
main.cpp::handle_sync_signal(): got a sync signal: 5
main.cpp::start_proc(): [debug] Printing args for process proc_t:
main.cpp::start_proc(): [0]: |proc_t|
main.cpp::start_proc(): [1]: |9|
main.cpp::start_proc(): [2]: |19267585|
main.cpp::start_proc(): [3]: |23494656|
main.cpp::wait_for_signal_sync(): waiting for signal sync status from process proc_t...
main.cpp::handle_sync_signal(): got a sync signal: 6
main.cpp::start_proc(): [debug] Printing args for process proc_d:
main.cpp::start_proc(): [0]: |proc_d|
main.cpp::start_proc(): [1]: |19300354|
main.cpp::start_proc(): [2]: |23527425|
main.cpp::start_proc(): [3]: |6076|
main.cpp::wait_for_signal_sync(): waiting for signal sync status from process proc_d...
proc_t: buffer: nula-pr
Attached shared memory by proc_d
Created socket by proc_d
main.cpp::handle_sync_signal(): got a sync signal: 7
main.cpp::start_proc(): [debug] Printing args for process proc_s:
main.cpp::start_proc(): [0]: |proc_s|
main.cpp::start_proc(): [1]: |19267585|
main.cpp::start_proc(): [2]: |23494656|
main.cpp::start_proc(): [3]: |19300354|
main.cpp::start_proc(): [4]: |23527425|
Connected to socket by proc_d
main.cpp::wait_for_signal_sync(): waiting for signal sync status from process proc_s...
main.cpp::handle_sync_signal(): got a sync signal: 8
proc_t: buffer: desat-pr
proc_t: buffer: jeden-pr
Wrote to socket by proc_d
Wrote to socket by proc_d
proc_serv2: bytes_received: 16
proc_t: buffer: dvadsat-pr
proc_serv2: buffer: nula-pr-s-serv1
proc_serv2: bytes_received: 17
proc_serv2: buffer: desat-pr-s-serv1
Wrote to socket by proc_d
proc_t: buffer: dva-pr
proc_serv2: bytes_received: 17
proc_serv2: buffer: jeden-pr-s-serv1
Wrote to socket by proc_d
proc_t: buffer: tridsat-pr
proc_serv2: bytes_received: 19
proc_serv2: buffer: dvadsat-pr-s-serv1
proc_t: buffer: tri-pr
Wrote to socket by proc_d
proc_serv2: bytes_received: 15
proc_serv2: buffer: dva-pr-s-serv1
Wrote to socket by proc_d
proc_t: buffer: styridsat-pr
proc_serv2: bytes_received: 19
proc_serv2: buffer: tridsat-pr-s-serv1
proc_t: buffer: styri-pr
Wrote to socket by proc_d
proc_t: buffer: patdesiat-pr
Wrote to socket by proc_d
proc_serv2: bytes_received: 15
Wrote to socket by proc_d
proc_serv2: buffer: tri-pr-s-serv1
proc_serv2: bytes_received: 21
proc_serv2: buffer: styridsat-pr-s-serv1
proc_serv2: bytes_received: 17
Wrote to socket by proc_d
proc_serv2: buffer: styri-pr-s-serv1
proc_serv2: bytes_received: 21
proc_serv2: buffer: patdesiat-pr-s-serv1
P1: Finished
P2: Finished
d off
```