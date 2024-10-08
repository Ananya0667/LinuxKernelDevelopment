# Linux Kernel Development

Welcome to the Linux Kernel Development project. This repository contains a collection of experiments and assignments.

## Context Switch Tracker and On Demand Signal Generator - Table of Contents

1. [Linux Installation: Virtual Machine Setup](#linux-installation-virtual-machine-setup)
2. [Building the Linux Kernel](#building-the-linux-kernel)
3. [Context Switch Tracker](#context-switch-tracker)
4. [On Demand Signal Generator using a Kernel Module](#on-demand-signal-generator-using-a-kernel-module)
5. [Extra Experimentation and Learning](#extra-experimentation-and-learning)

# Linux CPU Scheduler for Real-Time Systems - Table of Contents

1. [Introduction](#introduction)
2. [System Calls](#system-calls)
3. [Implementation Details](#implementation-details)
4. [Scheduler API](#scheduler-api)
5. [Schedulability Test](#schedulability-test)
6. [User Application](#user-application)


# Linux Character Device Driver - Table of Contents

1. [Intro_duction](#intro_duction)
2. [File Operations](#file-operations)
3. [Initialization and Cleanup](#initialization-and-cleanup)
4. [Usage](#usage)
5. [Building and Testing](#building-and-testing)
## Linux Installation: Virtual Machine Setup

This section guides setting up a Linux environment using Oracle VM VirtualBox. It includes downloading the Ubuntu Server ISO and configuring a virtual machine with specific hardware requirements.

## Building the Linux Kernel

Instructions on how to build the Linux kernel from source. This includes:

- Installing necessary packages such as `git`, `fakeroot`, and `build-essential`.
- Cloning the Linux kernel source from the official repository.
- Commands for configuring and compiling the kernel using `make` and resolving common errors encountered during the build.

## Context Switch Tracker

Development of custom system calls for tracking context switches within the Linux kernel. 

- Creating a directory for system calls and adding new files for each call.
- Details on compiling and integrating these system calls into the kernel.
- Implementing a linked list to manage processes and track their context switches.

## On Demand Signal Generator using a Kernel Module

This module allows processes to communicate via signals using the `/proc` filesystem:

- Description of creating a `/proc` file to facilitate inter-process communication.
- Implementation of a workqueue to handle signal operations.
- Detailed explanation of the module's functions like initialization, operation, and cleanup.

## Extra Experimentation and Learning

- Exploration of different methods to monitor context switch counts using the `rusage` structure and `/proc` filesystem.
- Discussion on the practical differences and use cases for `sleep()` and `sched yield()` system calls.


--------------------------------------------------------------------------------------------------------------------------------------------------


# Linux CPU Scheduler for Real-Time Systems

The implementation of a Linux CPU Scheduler based on the Rate-Monotonic (RMS) and Deadline-Monotonic (DMS) scheduling algorithms. I explored how to leverage the Linux kernel API to create a scheduler that handles real-time tasks with specific deadlines and periods.


## Introduction

Simulated a real-time scheduler based on the Liu and Layland periodic task model. It utilizes static priority scheduling where tasks with shorter periods are given higher priority. The implementation includes both Rate-Monotonic Scheduling (RMS) and Deadline-Monotonic Scheduling (DMS).

## System Calls

Implemented custom system calls include:
- `sys_register_dm`: Registers a task with given period, deadline, and execution time.
- `sys_yield`: Called by a task to release the CPU voluntarily until the next period.
- `sys_remove`: Removes a task and frees associated resources.
- `sys_list`: Lists all registered tasks with their parameters.

## Implementation Details

This section elaborates on the technical details of the scheduler:
- **Task States**: Tasks can be in READY, RUNNING, or SLEEPING state.
- **Process Control Block**: Custom PCBs are augmented with scheduling parameters.
- **Task Registration and Removal**: How tasks are added to the scheduler and subsequently removed.

## Scheduler API

Describes the kernel scheduler functions used:
- `schedule()`: Used by tasks to voluntarily yield the CPU.
- `wake_up_process()`: Wakes up a sleeping process.
- `sched_setscheduler()`: Sets the scheduling policy and priority for processes.

## Schedulability Test

Details the function `compute_interference`, which ensures all tasks can complete within their deadlines to maintain system schedulability.

## User Application

Provides a simple user-space application that demonstrates how to interact with the scheduler:
- Registers real-time processes.
- Performs computational tasks.
- Yields CPU based on scheduling requirements.


--------------------------------------------------------------------------------------------------------------------------------------------------
# Linux Character Device Driver

A Linux character device driver that demonstrates the basic mechanisms for handling read, write, open, and release operations on a character device. The driver is designed to provide practical insights into device file operations and interaction between kernel space and user space.

## Intro_duction

Implemented a simple character device driver. It covers the fundamental aspects of device driver operations in Linux, focusing on:

- Implementing file operations (read, write, open, and release).
- Dynamically creating device files and managing device classes.
- Properly handling module initialization and cleanup.

## File Operations

The driver defines several callback functions:

- `my_open`: Handles opening operations of the device file.
- `my_release`: Manages closing operations.
- `my_read`: Retrieves data from the device to the user buffer.
- `my_write`: Sends data from the user buffer to the device.

Each operation is linked to the typical system calls used by character devices.

## Initialization and Cleanup

- **Initialization (`my_init`):** 
  - Dynamically allocates device numbers.
  - Registers the device class.
  - Creates device files accessible from user space.

- **Cleanup (`my_exit`):**
  - Unregisters device numbers and destroys device classes and files.

## Usage

Detailed function descriptors for each part of the driver are provided, including how data transactions are handled and errors are managed.

## Building and Testing

Instructions for compiling and loading the module, as well as running the provided writer and reader test programs:

```bash
gcc writer.c -o writer
./writer /dev/mydevice1 "message to write"

gcc reader.c -o reader
./reader /dev/mydevice0

