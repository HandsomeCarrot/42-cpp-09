*This project has been created as part of the 42 curriculum by vpoka.*

# CPP09 — STL

A C++98 project from the 42 curriculum focused on practical use of the Standard Template Library (STL), input validation, and algorithmic problem-solving.

## Table of contents

- [Description](#description)
- [Instructions](#instructions)
- [Resources](#resources)
- [What this project demonstrates](#what-this-project-demonstrates)
- [Technical constraints](#technical-constraints)
- [Repository structure](#repository-structure)
- [Focus areas by exercise](#focus-areas-by-exercise)
- [Testing](#testing)
- [Troubleshooting](#troubleshooting)
- [Status](#status)
- [License](#license)

## Description

CPP09 is the final module of the 42 C++ Common Core sequence. The project emphasizes writing clean, reliable C++98 code while using standard containers appropriately and following strict subject constraints.

This module is split into three exercises:

* **ex00 — Bitcoin Exchange**
  Parse historical exchange-rate data from CSV, validate user input, and compute Bitcoin values for requested dates.
* **ex01 — Reverse Polish Notation**
  Evaluate mathematical expressions written in postfix notation with proper error handling.
* **ex02 — PmergeMe**
  Implement and benchmark a merge-insert sorting approach (Ford-Johnson) using two different STL containers.

| Exercise | Executable | Containers used |
| --- | --- | --- |
| [ex00](ex00/) — Bitcoin Exchange | `btc` | `std::map<std::string, double>` |
| [ex01](ex01/) — Reverse Polish Notation | `RPN` | `std::stack<int, std::list<int> >` |
| [ex02](ex02/) — PmergeMe | `PmergeMe` | `std::vector` and `std::deque` |

## Instructions

### Prerequisites

- A C++ compiler available as `c++`, supporting `-std=c++98`.
- GNU Make and standard Unix shell utilities. The Makefiles use `-Wall -Wextra -Werror -std=c++98`; no external libraries are required.
- For the test scripts: Bash (4+ for the PmergeMe harness's associative arrays), standard Unix text utilities, and `shuf`/`seq` for PmergeMe. Valgrind is optional for Bitcoin Exchange memory checks.

The repository does not specify minimum compiler, Make, or Valgrind versions.

### Build

Run these commands from the repository root. Each exercise has its own Makefile; there is no root Makefile.

```bash
make -C ex00
make -C ex01
make -C ex02
```

Each Makefile provides `all`, `clean` (remove build files), `fclean` (also remove the executable), and `re` (rebuild). For example:

```bash
make -C ex00 clean
make -C ex01 fclean
make -C ex02 re
```

All three also provide `debug` (`-g -DDEBUG`) and `sanitize` (address, leak, and undefined-behavior sanitizers; requires compiler support).

### ex00 — Bitcoin Exchange

From the repository root:

```bash
(cd ex00 && ./btc tests/input.txt)
```

`btc` takes exactly one argument: the path to an input file. It loads `data.csv` from the **current working directory**, so run it inside `ex00`.

The input must begin with `date | value`, followed by records using the literal separator ` | `:

```text
date | value
2011-01-03 | 3
2011-01-09 | 1
```

Dates must be calendar-valid `YYYY-MM-DD` values, and Bitcoin amounts must be between **0 and 1000 inclusive**. Missing database dates use the closest earlier rate; dates before the earliest database entry produce an error. Invalid records are reported on standard error while later records continue to be processed.

The bundled sample deliberately includes invalid records. Successful records use the format `date => amount = converted value`, preceded by an informational line.

### ex01 — Reverse Polish Notation

From the repository root:

```bash
./ex01/RPN "8 9 * 9 - 9 - 9 - 4 - 1 +"  # 42
./ex01/RPN "7 7 * 7 -"                    # 42
./ex01/RPN "1 2 * 2 / 2 * 2 4 - +"         # 0
```

Pass the whole expression as **one quoted argument**. Tokens are whitespace-separated single digits (`0`–`9`) or operators (`+`, `-`, `*`, `/`). Arithmetic uses integer results, so division truncates toward zero. Parentheses, decimal operands, and multi-digit operands are rejected.

Invalid expressions, division by zero, and results outside the `int` range produce an error on standard error and exit with status `1`.

### ex02 — PmergeMe

From the repository root:

```bash
./ex02/PmergeMe 3 5 9 7 4
```

Pass at least **two numbers**, each as a separate argument. The subject requests positive integers; this implementation also accepts zero and duplicates. Negative values, malformed tokens, and values above `std::numeric_limits<int>::max()` are rejected.

The program prints the original sequence, the sorted sequence, and CPU timing for `std::vector` and `std::deque` in seconds, milliseconds, and microseconds. Timers surround each container's sort, including its internal rearrangement work; initial argument parsing and container population occur before timing.

Example with 3000 distinct numbers (requires `shuf` and `tr`):

```bash
./ex02/PmergeMe $(shuf -i 1-100000 -n 3000 | tr '\n' ' ')
```

## Resources

- **Donald E. Knuth, *The Art of Computer Programming*, Volume 3: Sorting and Searching**, merge-insertion sorting, page 184 — the Ford-Johnson reference cited by the subject.
- **cppreference C++ standard-library reference:** entries for `std::map::upper_bound`, `std::stack`, `std::list`, `std::vector`, `std::deque`, and `std::clock`. Consult the C++98 behavior when reading modern documentation.
- **GNU Make manual:** targets, automatic variables, and dependency tracking.
- **Valgrind User Manual, Memcheck:** memory and file-descriptor checks used by the Bitcoin Exchange harness.

### AI usage

AI was used to help write and improve this README and project documentation, assist with performance optimisation, and support understanding of the Ford-Johnson merge-insertion algorithm used in PmergeMe (`ex02`).

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
* Containers cannot be reused across exercises; ex00 and ex01 require at least one, and ex02 requires at least two different containers
* The subject requires PmergeMe to handle at least 3000 different integers
* External libraries, Boost, and C++11 or later features are forbidden by the subject

## Repository structure

```text
cpp09/
├── README.md
├── ex00/   # Bitcoin Exchange: main.cpp, BitcoinExchange.cpp/.hpp, Makefile
│   ├── data.csv
│   └── tests/   # Sample input, fixtures, functional and Valgrind harness
├── ex01/   # RPN: main.cpp, RPN.cpp/.hpp, Makefile
└── ex02/   # PmergeMe: main.cpp, PmergeMe.cpp/.hpp, Makefile
    └── test/    # Debug-output-based sorting and comparison harness
```

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

## Testing

### Bitcoin Exchange

Build the normal executable, then run the fixture-based harness from the repository root:

```bash
make -C ex00
bash ex00/tests/run.sh functional
```

For memory and file-descriptor checks (requires Valgrind):

```bash
bash ex00/tests/run.sh valgrind
bash ex00/tests/run.sh all
```

Fixtures cover file/header errors, date and numeric validation, and exact/earlier-date lookups. The harness compares output and exit codes in temporary case directories under `ex00/tests/tmp`, which it removes on exit.

### PmergeMe

The harness reads sorting and comparison-count lines emitted only by a **debug build**. Run it from its own directory because its binary path is relative:

```bash
make -C ex02 debug
(cd ex02/test && bash test_pmergeme.sh)
```

It checks both containers' sorted-status reports and comparison counts, runs fixed edge cases and randomized sequences up to 3000 elements, and summarizes timings. Failures are logged in its working directory. Use the normal build for ordinary timing comparisons; debug tracing adds overhead.

## Troubleshooting

- **`btc` cannot load its database:** run it inside `ex00`, where `data.csv` is located. Providing an input path alone does not change the database path.
- **`btc` rejects a header or separator:** use the exact header `date | value` and separator ` | `, including spaces.
- **`RPN` reports an argument/token error:** quote the whole expression and separate single-digit operands and operators with whitespace.
- **PmergeMe tests report missing output lines:** build with `make -C ex02 debug` and run the script inside `ex02/test`.

## Status

* **Status:** Completed
* **Final grade:** **100/100 points**
* **Evaluation date:** 2026-04-15
