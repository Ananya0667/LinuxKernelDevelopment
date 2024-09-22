# Linux Kernel Dev

Welcome to the Linux Kernel Dev project! This repository hosts a collection of Linux kernel development assignments and experiments focusing on kernel modules, system calls, and process management. The project aims to provide insights into kernel operations and practical hands-on experience with system-level programming in Linux.

## Table of Contents

- [Project Overview](#project-overview)
- [Setup and Installation](#setup-and-installation)
- [Kernel Modules and System Calls](#kernel-modules-and-system-calls)
- [Scheduling Algorithms](#scheduling-algorithms)
- [Context Switch Tracking](#context-switch-tracking)
- [On-Demand Signal Generator](#on-demand-signal-generator)
- [How to Use](#how-to-use)
- [Contributing](#contributing)
- [License](#license)

## Project Overview

This project encompasses multiple assignments that delve into the Linux kernel's functionalities, such as creating kernel modules, system calls, and various methods of process scheduling. It includes implementations of Rate-Monotonic (RMS) and Deadline-Monotonic Scheduling (DMS), a Context Switch Tracker, and an On-Demand Signal Generator using the Linux kernel.

## Setup and Installation

Detailed instructions on setting up a virtual machine for kernel development, building the Linux kernel, and configuring system parameters.

### Prerequisites
- Oracle VM VirtualBox Manager
- Ubuntu Server 22.04.03
- Required tools and libraries (e.g., `git`, `make`, `gcc`)

### Configuration
- Setup a virtual machine with 4 CPU cores and 8GB RAM.
- Install the necessary development tools and build the Linux kernel from source.

## Kernel Modules and System Calls

Introduction to writing and implementing custom kernel modules and system calls. This section includes:
- Creation of dummy system calls for educational purposes.
- Implementation of context switch tracking mechanism within the kernel.

## Scheduling Algorithms

Implementation of scheduling algorithms using the Linux Kernel API, which includes:
- Deadline Monotonic (DM) and Rate Monotonic (RM) scheduling.
- System calls for task registration and management.

## Context Switch Tracking

Development of a system call to monitor and record context switches within the Linux kernel, aiding in performance analysis and debugging.

## On-Demand Signal Generator

A kernel module that allows processes to send signals to each other on-demand using a virtual file in the `/proc` directory. This module includes:
- Implementation of a workqueue to handle signal delivery.
- User-space applications to interact with the kernel module.

## How to Use

Examples and explanations on how to compile and run the modules, interact with the implemented system calls, and utilize the user applications for testing.

## Contributing

Guidelines for contributing to the project, including how to submit issues, feature requests, and pull requests.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
