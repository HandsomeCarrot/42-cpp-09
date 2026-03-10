# CPP09 — STL

A C++98 project from the 42 curriculum focused on practical use of the Standard Template Library (STL), input validation, and algorithmic problem-solving.

## Overview

CPP09 is the final module of the 42 C++ Common Core sequence. The project emphasizes writing clean, reliable C++98 code while using standard containers appropriately and following strict subject constraints.

This module is split into three exercises:

* **ex00 — Bitcoin Exchange**
  Parse historical exchange-rate data from CSV, validate user input, and compute Bitcoin values for requested dates.
* **ex01 — Reverse Polish Notation**
  Evaluate mathematical expressions written in postfix notation with proper error handling.
* **ex02 — PmergeMe**
  Implement and benchmark a merge-insert sorting approach (Ford-Johnson) using two different STL containers.

## What this project demonstrates

* C++98 development under strict compilation rules (`-Wall -Wextra -Werror -std=c++98`)
* Robust file parsing and input validation
* Careful error handling and edge-case management
* Practical use of STL containers in problem-specific contexts
* Performance comparison between different container choices
* Writing code that is explainable during peer evaluation and maintainable afterward

## Technical constraints

This project is developed under the 42 C++ module rules:

* Standard: **C++98**
* Compiler flags: **`-Wall -Wextra -Werror -std=c++98`**
* STL usage is mandatory in this module
* Each exercise must use the required container constraints from the subject

## Repository structure

```text
cpp09/
├── ex00/   # Bitcoin Exchange
├── ex01/   # Reverse Polish Notation
└── ex02/   # PmergeMe
```

## Build and usage

Each exercise contains its own `Makefile` and is built separately.

### ex00 — Bitcoin Exchange

```bash
cd ex00
make
./btc input.txt
```

Expected input format:

```text
date | value
2011-01-03 | 3
2011-01-09 | 1
```

Notes:

* executable name: `btc`
* takes a file path as its single argument
* validates dates and values, then prints the computed exchange result for each valid line
* if a date is missing from the database, the closest lower available date must be used

### ex01 — Reverse Polish Notation

```bash
cd ex01
make
./RPN "8 9 * 9 - 9 - 9 - 4 - 1 +"
```

More examples:

```bash
./RPN "7 7 * 7 -"
./RPN "1 2 * 2 / 2 * 2 4 - +"
```

Notes:

* executable name: `RPN`
* takes the full expression as a single command-line argument
* supports the operators `+`, `-`, `*`, and `/`
* invalid expressions must produce an error on standard error

### ex02 — PmergeMe

```bash
cd ex02
make
./PmergeMe 3 5 9 7 4
```

Example with a larger generated input:

```bash
./PmergeMe `shuf -i 1-100000 -n 3000 | tr "
" " "`
```

Notes:

* executable name: `PmergeMe`
* takes a sequence of positive integers as command-line arguments
* sorts the sequence with the Ford-Johnson merge-insert approach
* displays the sequence before sorting, after sorting, and timing information for two different STL containers

## Focus areas by exercise

### ex00 — Bitcoin Exchange

* CSV data loading
* date parsing and validation
* numeric input validation
* lookup of the closest lower available date
* clear handling of malformed input and invalid values

### ex01 — Reverse Polish Notation

* stack-based evaluation
* token validation
* operator application order
* safe error reporting on invalid expressions

### ex02 — PmergeMe

* merge-insert / Ford-Johnson sorting logic
* comparing behavior across two STL containers
* measuring and displaying execution time
* handling larger integer input ranges correctly

## Results

* **Status:** *In progress*
* **Final grade:** *TBD*
* **Evaluation date:** *TBD*
* **Notes / feedback:** *TBD*
