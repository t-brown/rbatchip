# rbatchip
RPC call to create a MNP hostfile in AWS Batch

## Introduction
When using [MPI](https://en.wikipedia.org/wiki/Message_Passing_Interface) to run multi-node parallel jobs on [AWS Batch](https://docs.aws.amazon.com/batch/latest/userguide/multi-node-parallel-jobs.html) one needs to generate a host file.

There are two programs within this repository, `rbatchipd` the RPC server
program and `rbatchip` the RPC client program. The server program should
be run on the node that will be executing the MPI program (e.g. the node
calling `mpiexec`). While the client program should be executed on every
node that will be participating in the job.

## Prerequisites

+  [C compiler](https://gcc.gnu.org/)
+  [rpcgen](https://en.wikipedia.org/wiki/RPCGEN)
+  [make](https://www.gnu.org/software/make/)

Also if on a recent GNU libc you will need [libtirpc](https://sourceforge.net/projects/libtirpc/).

## Building / Installation

To install `rbatchip` with the default options into `/usr/local`:

    make
    make install PREFIX=/usr/local


