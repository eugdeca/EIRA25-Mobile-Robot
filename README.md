Final Project for the "Introduction to Computer Science" Exam.

This repository contains a C++ simulation of a remotely controlled robotic arm. The project implements a Client-Server architecture using TCP/IP sockets, allowing a number of remote "Joypads" or "Safety Buttons"(Client) to control the movements and the gripper of a simulated robot (Server).

The core of the system relies on multithreading to manage concurrent tasks such as arm movement, gripper actuation, and battery consumption in real-time.
