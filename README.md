# Mobile Robot Remote Control Simulation

![C++](https://img.shields.io/badge/C++-11%2F14%2F17-blue.svg)
![Architecture](https://img.shields.io/badge/Architecture-Client%2FServer-success.svg)
![Network](https://img.shields.io/badge/Network-TCP%2FIP_Sockets-orange.svg)

## Project Overview
Final Project for the **EIRA** (Introduction to Computer Science / Robotics) Exam.

This repository contains a complete C++ simulation of a remotely controlled mobile robot equipped with a robotic arm. The system is built upon a robust **Client-Server architecture** using **TCP/IP sockets**, allowing multiple remote clients (such as a full-featured "Joypad" or a limited "Safety Button") to interact with and control the simulated robot (Server).

The core of the system relies heavily on **multithreading** to manage concurrent tasks in real-time, such as chassis movement, arm tilting, gripper actuation, and continuous battery consumption.

## Highlighted Skills & Core Concepts
This project was designed to demonstrate advanced C++ programming techniques:

* **Object-Oriented Programming (OOP):** the codebase is heavily modularized with a strict separation of concerns between the robot's internal logic, mechanisms (Base, Arm, Battery), and control interfaces. Each class autonomously manages its own state to ensure data consistency.
* **Concurrent Programming (Multithreading):** implementation of safe, concurrent execution threads to handle simultaneous physical simulations (e.g., the battery draining while the arm moves) and non-blocking network request handling.
* **Socket Programming:** custom implementation of a TCP/IP networking layer to handle reliable, structured packet transmission between the remote controllers (Clients) and the robot (Server).


## System Architecture
* **The Server (Robot):** maintains the physical state of the mobile base, arm, gripper, and battery. It continuously listens for incoming connections and executes commands asynchronously.
* **The Clients (Controllers):**
  * *Joypad:* can send movement vectors, actuate the arm/gripper, and request telemetry (position, battery status).
  * *Safety Button:* a restricted client that can only send emergency commands (`ACTIVATE`, `DEACTIVATE`, `STOP`).

## Academic Integrity Disclaimer
This repository contains my personal, original implementation of the project's logic and classes. 

To strictly respect the university's academic integrity policies and the professor's copyright:
1. The original assignment PDF containing the full requirements has not been uploaded.
2. The `main.cpp` file for the Client application, which was provided by the professor as boilerplate code, has been intentionally excluded from this public repository. 
The system is built so that my client-side classes seamlessly interface with the provided (but omitted) main executable.

## Author
* **Eugenio Delli Carri**
