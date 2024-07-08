# ChatApplication

## Prerequisites

The following dependencies are required to compile and run the program:

- cmake
- gcc, g++
- boost

**Ensure that TCP traffic on port 12345 is permitted between the Server and Client machines.**

## Setup

Copy source files into the Linux machine:

```sh
scp -r ChatApplication/ username@hostname:/home/username

```

Ensure cmake is installed:

```sh
# If not installed run the following commands:

# For Debian-based:
sudo apt update
sudo apt install cmake

# For CentOS or RHEL:
sudo yum update
sudo yum install cmake

cmake --version
```

Ensure development tools and compilers are installed:

```sh

# For Debian-based:
sudo apt update
sudo apt install build-essential

# For CentOS or RHEL:
sudo yum update
sudo yum groupinstall "Development Tools"

gcc --version
g++ --version
```

Ensure boost library is installed:

```sh
# For Debian-based:
sudo apt update
sudo apt install libboost-all-dev
dpkg -l | grep libboost

# For CentOS or RHEL:
sudo yum update
sudo yum install boost-devel
rpm -qa | grep boost
```

Generate build files:

```sh
mkdir build
cd build

cmake ..
```

Build the project:

```sh
make
```

## Testing

Run 2 instances of ChatApplication, either in the same machine or in 2 different Linux instances.

```sh
./chat_application server
./chat_application client
```

On the client, enter the server's ip/hostname into the console.

This application has been tested between:

- 1 Windows Linux Subsystem (localhost)
- 2 Amazon Linux instances
- 2 Ubuntu instances

### Guide

- To send a chat message, type your message and press enter.
- The client will receive the message and send an acknowledgement back to you (the server).
- You will receive the ack and the Round Trip Time (RTT) will be displayed.
- Likewise, if you receive a chat message, you (the server) will send an acknowledgement back to the client.

## Limitations

- Reading of user input stops at the `\n` delimiter. This means that pasting a text with multiple lines would cause it to be read line by line.
- Also, pasting multiple lines would only enter all lines except the last one, so the user would have to press enter again to input the last line. This is a console/terminal limitation.
- There is no GUI, this application is text-based only.
