# 42 Exam Rank 04 - Solved & Explained 🎓

![Status](https://img.shields.io/badge/Status-Completed-success?style=for-the-badge&logo=42)
![Language](https://img.shields.io/badge/Language-C-blue?style=for-the-badge&logo=c)
![Platform](https://img.shields.io/badge/Platform-Unix-lightgrey?style=for-the-badge&logo=linux)

This repository contains my personal implementations for the **Exam Rank 04** at 42 School. This exam is a significant milestone that tests advanced C concepts across two main domains: **Unix Process Management** (pipes, forks, signals) and **Recursive Parsing Algorithms** (ASTs, JSON, Math).

## 📂 Repository Structure

The exam consists of randomly selected exercises from the following pool, divided by difficulty level:

| Level | Exercise | Domain | Description |
| :---: | :--- | :--- | :--- |
| **1** | `ft_popen.c` | Unix/Pipes | Re-implementation of the standard `popen` function. |
| **1** | `picoshell.c` | Unix/Pipes | A minimal shell capable of executing pipelines. |
| **1** | `sandbox.c` | Unix/Signals | A safety wrapper to execute functions with timeouts/crash detection. |
| **2** | `vbc.c` | Algo/AST | (Volume Basic Calculator) A parser for arithmetic expressions. |
| **2** | `argo.c` | Algo/JSON | A recursive descent parser for JSON files. |

---

## 🛠️ Level 1: Processes, Pipes & Signals

### 🔹 ft_popen
A low-level recreation of `<stdio.h>`'s `popen`.
* **Goal:** Launch a process via a pipe and return a file descriptor to read from or write to it.
* **Key Concepts:** `pipe()`, `fork()`, `dup2()`, `execvp`.
* **Logic:** Depending on the mode ('r' or 'w'), connects the child's STDOUT or STDIN to a pipe and returns the corresponding end to the parent.

### 🔹 picoshell
A simplified shell execution engine.
* **Goal:** Execute a series of commands chained by pipes (`cmd1 | cmd2 | cmd3`).
* **Key Concepts:** Iterative forking, file descriptor management (avoiding leaks), `waitpid`.
* **Logic:** Uses a loop to fork processes, passing the read-end of the previous pipe to the input of the next process via `dup2`.

### 🔹 sandbox
A "safe mode" executor for C functions.
* **Goal:** Run a specific function in a child process and determine if it behaves correctly, crashes (segfault), or times out.
* **Key Concepts:** `fork()`, `alarm()` (timeouts), `sigaction` (handling `SIGALRM`), `waitpid` macros (`WIFSIGNALED`, `WEXITSTATUS`).
* **Logic:** The parent sets an alarm. If the child takes too long, the `SIGALRM` handler interrupts the wait, and the parent kills the child process.

---

## 🧠 Level 2: Algorithms & Parsing

### 🔸 vbc (Volume Basic Calculator)
A mathematical expression evaluator using an Abstract Syntax Tree (AST).
* **Goal:** Parse strings like `1 + 2 * (3 + 4)` respecting operator precedence.
* **Key Concepts:**
    * **AST Nodes:** Creating a tree where leaves are numbers and nodes are operations.
    * **Recursive Descent:** Three levels of parsing (`Expression` -> `Term` -> `Factor`) to handle precedence (`*` before `+`).
    * **Parentheses:** Recursive calls to reset priority.

### 🔸 argo (JSON Parser)
A parser that reads a data stream and constructs a JSON structure in memory.
* **Goal:** Parse JSON containing Integers, Strings, and Maps (Objects).
* **Key Concepts:**
    * **Lookahead:** Using `getc` and `ungetc` (peek) to decide the data type.
    * **Tagged Unions:** Using a `struct` with an `enum` to handle dynamic types in C.
    * **Memory Management:** Using `realloc` to handle Maps with an unknown number of keys.
    * **Serialization:** A pretty-printer function to output the parsed structure back to string format.

---

## 🚀 Usage & Compilation

To test these exercises, compile them with the standard flags.
*Note: Level 1 exercises usually require a `main.c` (not provided in the exam subject) to run, while Level 2 exercises often include a main entry point.*

### Compiling VBC
```bash
gcc -Wall -Wextra -Werror vbc.c given.c -o vbc
./vbc "2 * (3 + 4)"
# Output: 14
```

### Compiling Argo
```bash
gcc -Wall -Wextra -Werror argo.c given.c -o argo
echo '{"key": "value", "id": 42}' | ./argo /dev/stdin
# Output: {"key":"value","id":42}
```