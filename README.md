# dos_tool 

onsole-based utility designed to test network resilience against DoS attacks by generating controlled traffic.

---

## Installation

This project uses the standard C++ compiler (`g++`) and the **GNU Make** build system.

### Prerequisites

To successfully build this project on your system, you'll need:

* **Git**
* **GNU Make**
* **A C++ Compiler** (with C++17 support)

### Step 1: Clone the Repository

First, clone the project from GitHub using the SSH address (if you configured it):

```bash
git clone https://github.com/ekkor13/dos_tool/
cd dos_tool
make
make install
dos_tool --help
