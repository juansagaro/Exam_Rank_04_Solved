# 42 Exam Rank 04 - Solved & Explained 🎓

![Status](https://img.shields.io/badge/Status-Completed-success?style=for-the-badge&logo=42)
![Language](https://img.shields.io/badge/Language-C-blue?style=for-the-badge&logo=c)
![Platform](https://img.shields.io/badge/Platform-Unix-lightgrey?style=for-the-badge&logo=linux)

This repository contains my implementations for the **Exam Rank 04** at 42 School. The exam focuses on advanced Unix process management, inter-process communication (pipes), safety sandboxing, and recursive parsing algorithms.

## 📂 Repository Structure

The exam is divided into two levels of difficulty:

| Level | Exercise | Description |
| :---: | :--- | :--- |
| **1** | `ft_popen.c` | Re-implementation of the standard `popen` library function. |
| **1** | `picoshell.c` | A minimal shell to execute commands and pipelines. |
| **1** | `sandbox.c` | A function to execute code in a safe, isolated child process. |
| **2** | `vbc.c` | (Volume Basic Calculator) An expression evaluator using ASTs. |

---

## 🛠️ Level 1: Processes & Pipes

### 🔹 ft_popen
A recreation of the `<stdio.h>` function `popen`.
* **Goal:** Launch a process via a pipe and return a file stream.
* **Key Concepts:** `pipe()`, `fork()`, `dup2()`, `execve()`, and `fdopen()`.
* **Challenge:** Correctly connecting the file descriptors to the standard input/output of the child process depending on the mode ("r" or "w").

### 🔹 picoshell
A simplified shell execution engine.
* **Goal:** Execute a series of commands chained by pipes (`|`).
* **Key Concepts:** Iterative forking, pipe management to prevent file descriptor leaks, and `waitpid`.
* **Challenge:** Passing the output of one process to the input of the next without blocking the execution.

### 🔹 sandbox
A safety wrapper for function execution.
* **Goal:** Execute a function in a child process and detect if it crashes or behaves badly (timeouts).
* **Key Concepts:** `fork()`, `waitpid()` status macros (WIFSIGNALED), and handling signals/timeouts (often using `alarm` or `setrlimit`).
* **Challenge:** Isolating the parent process from crashes (segfaults) occurring in the child.

---

## 🧠 Level 2: Algorithms & Parsing

### 🔸 vbc (Volume Basic Calculator)
A parser and evaluator for arithmetic expressions.
* **Goal:** Parse strings like `1 + 2 * (3 + 4)` and return the result.
* **Key Concepts:**
    * **Abstract Syntax Tree (AST):** Nodes representing Values, Additions, and Multiplications.
    * **Recursive Descent Parsing:** Handling operator precedence (`*` before `+`) and nested parentheses.
    * **Memory Management:** Cleanly freeing the tree structure even in case of syntax errors.

---

## 🚀 Usage & Compilation

To test any of these exercises, you can use the standard 42 compilation flags:

```bash
gcc -Wall -Wextra -Werror <filename>.c main.c -o <program_name>