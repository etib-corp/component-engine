# Component Engine

**A declarative UI component engine for Python, inspired by React but built with a C++ core.**

## Overview

Component Engine is a lightweight, high-performance library for building user interfaces in Python. It brings the **component-based architecture** and **reactive updates** of React to Python, but replaces JSX/TSX with Pythonic syntax and leverages a **C++ core** for performance-critical operations.

### Key Features

- **Declarative UI**: Define your UI as a hierarchy of reusable components.
- **Reactive Updates**: Efficiently update the DOM (or equivalent) in response to state changes.
- **Pythonic Syntax**: No JSX/TSX—use pure Python to describe your components.
- **C++ Core**: Performance optimizations for rendering and diffing.
- **Cross-Platform**: Designed to work with various backends (e.g., web, desktop, or embedded).

---

## Why Component Engine?

- **Familiar Paradigm**: If you know React, you’ll feel at home.
- **Python First**: Write components in Python, not JavaScript.
- **Performance**: C++ core ensures fast rendering and minimal overhead.
- **Extensible**: Plug in custom renderers for different platforms.

---

## Core Concepts

### 1. **Components**

Define UI pieces as Python classes. Each component manages its own state and lifecycle.

### 2. **Virtual DOM**

The C++ core handles efficient diffing and rendering, similar to React’s reconciliation.

### 3. **State Management**

Use hooks or class-based state to manage dynamic data.

### 4. **Event Handling**

Attach Python callbacks to user interactions.

---

## Project Structure

```plain
component_engine/
├── core/               # C++ core (rendering, diffing)
├── component_engine/   # Python bindings and public API
├── examples/           # Demo applications
└── tests/              # Unit and integration tests
```

---

## Building

### Prerequisites

- CMake (3.10 or higher)
- A C++ compiler (GCC or Clang)
- Python (3.6 or higher)

### Building the Project

1. Clone the repository:

```bash
   git clone https://github.com/etib-corp/component_engine.git --recurse-submodules
   cd component_engine
   ```

2. Configure the build

   ```bash
   cmake -S . -B build
   ```

3. Build the project:

   ```bash
   cmake --build build
   ```

---

## License

MIT
