# Embedded Monitor System
## Description
This project is a monitoring system for the embedded ARM developing board [TOPEET iTOP-4412 Elite Edition](http://topeetboard.com/Product/iTOP4412-ss.html) (the board) . The system contains application for the board itself (slave) and a host computer (host)  for remote monitoring.

The system supports realtime data sampling and image filming on the board, including value of a variable resistance and image data from an OV5640 camera. Two-way communication between the board and host computer is made possible by TCP protocol. User interfaces are powered by Qt.

## Usage
The “slave” app was build and tested on Ubuntu 18.04 desktop with arm-linux-gcc, Qt 4.7.1 and Qt Creator 5.14. The “”host” app was build and tested on macOS 10.15.7 with the latest version of Qt.

Since Qt is a cross-platform developing kit, building on other environments should be supported.

The following steps are necessary:
1. To bulid the “slave” app for the board, you need a cross-compiler, e.g. arm-linux-gcc, on a Linux system, e.g. [Ubuntu 18.04](https://ubuntu.com/download/desktop), with a pre-build Qt distribution, which also should be put in the file system of the board.
2. Configure kits in Qt Creator in Linux, with the following parameters:
	* Device: Generic Linux Device
	* C Compiler: arm-linux-gcc
	* C++ Compiler: arm-linux-gcc
	* Qt Version: Qt-4.7.1
3. Install [QWT](https://qwt.sourceforge.io/) on Linux system.
4. Install [Qt](https://www.qt.io/) on your host computer, **select QtCharts during installation**.
5. Download source code or clone the project, build with Qt Creator.
6. Put the executable file of “slave” app on the board and run, while also running the “host” app on your computer.
