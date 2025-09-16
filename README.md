# CVRPSEP for Row-and-Column-Generation with Cutting Planes

A high-performance implementation for computing lower bounds of Capacitated Vehicle Routing Problem (CVRP) instances using the Gurobi optimization solver and the CVRPSEP package for separation of cutting planes.

## Overview

This project implements a row-and-column-generation approach with cutting plane separation to solve CVRP instances. The implementation leverages:

- **Gurobi Optimizer**: For solving linear programming relaxations
- **CVRPSEP Library**: For separating various classes of valid inequalities including capacity cuts, comb inequalities, and multi-star inequalities
- **Euclidean 2D Instances**: Currently supports instances with `EDGE_WEIGHT_TYPE: EUC_2D`

### Key Features

- Automatic separation of multiple cut families (capacity, comb, multi-star, etc.)
- Support for standard TSPLIB format CVRP instances
- Comprehensive test suite with benchmark instances
- Cross-platform CMake build system
- Efficient memory management and constraint handling

## Prerequisites

### Required Dependencies

1. **Gurobi Optimizer** (version 8.0 or higher)
   - Academic license available for research purposes
   - Commercial license required for commercial use
   - Download from [Gurobi website](https://www.gurobi.com/)

2. **CMake** (version 3.20 or higher)
   - Required for building the project
   - Available at [cmake.org](https://cmake.org/)

3. **C++ Compiler** with C++17 support
   - GCC 7.0+ or Clang 5.0+ on Linux/macOS
   - Visual Studio 2017+ on Windows

## Installation

### 1. Clone the Repository

```bash
git clone https://github.com/yourusername/CVRPSEP-for-RCC.git
cd CVRPSEP-for-RCC
```

### 2. Build the Project

Using CMake (recommended):

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

The build process will:
- Automatically locate your Gurobi installation
- Compile the CVRPSEP library
- Create the executable `cvrp_solver`

## Configuration Details

### Gurobi Setup

The build system automatically locates Gurobi using the following precedence:

1. **CMake variable**: `-DGurobi_ROOT=/path/to/gurobi`
2. **Environment variable**: `export GUROBI_HOME=/path/to/gurobi`
3. **Standard paths**: `/opt/gurobi*`, `/Library/gurobi*`, `/usr/local/gurobi*`

### Data Directory

#### Default Configuration
Instance files should be placed in the `Instances/` directory. This path is automatically configured during compilation.

#### Custom Data Directory
Set the `CVRP_DATA_DIR` environment variable to use a different directory:

```bash
export CVRP_DATA_DIR="/path/to/your/instances"
./build/cvrp_solver
```

## Usage

### Basic Usage

Run the solver on the default instance:

```bash
./build/cvrp_solver
```

### Instance Format

The solver accepts CVRP instances in TSPLIB format with the following requirements:

- **Format**: `.vrp` files in TSPLIB format
- **Edge Weight Type**: `EUC_2D` (Euclidean 2D coordinates)
- **Node Coordinates**: Integer coordinates for distance calculations

#### Example Instance Structure
```
NAME: A-n54-k7
COMMENT: (Augerat et al, Min no of trucks: 7, Optimal value: 1167)
TYPE: CVRP
DIMENSION: 54
EDGE_WEIGHT_TYPE: EUC_2D
CAPACITY: 100
NODE_COORD_SECTION
1 40 50
2 45 68
...
DEMAND_SECTION
1 0
2 10
3 7
...
DEPOT_SECTION
1
-1
EOF
```

### Modifying Test Instances

Edit `RCC_CVRP/main.cpp` to change which instances are processed:

```cpp
// Single instance
const char* files[] = { "X-n261-k13" };

// Multiple instances
const char* files[] = {
    "A-n54-k7", "A-n64-k9", "A-n80-k10",
    "B-n50-k8", "B-n66-k9", "B-n68-k9"
};
```

## Project Structure

```
CVRPSEP-for-RCC/
├── CMakeLists.txt              # Main build configuration
├── cmake/
│   └── FindGurobi.cmake        # Gurobi detection script
├── RCC_CVRP/
│   ├── main.cpp                # Main solver implementation
│   ├── load.h                  # Instance loading utilities
│   ├── base_model.h            # Gurobi model setup
│   └── cvrpsep/               # CVRPSEP library
│       ├── capsep.h/.cpp      # Capacity cut separation
│       ├── combsep.h/.cpp     # Comb inequality separation
│       ├── cnstrmgr.h/.cpp    # Constraint manager
│       └── ...                # Other separation routines
└── Instances/                 # Test instance directory
    ├── *.vrp                  # CVRP instance files
    └── *.sol                  # Known optimal solutions
```

## Benchmark Instances

The project includes comprehensive benchmark instances from various sources:

### Classical Benchmark Sets
- **Augerat et al. (A, B, E, M, P series)**: Standard CVRP benchmarks
- **Christofides & Eilon (E series)**: Classic test instances

### Large-scale Instances
- **ORTEC series**: Industrial-scale problems
- **Loggi series**: Real-world logistics instances
- **X series**: Large benchmark instances

### Real-world Instances
- **Antwerp, Leuven**: European city delivery problems

## Algorithm Details

### Solution Approach

1. **Initial LP Relaxation**: Solve the basic CVRP linear programming relaxation
2. **Cut Generation**: Use CVRPSEP to identify violated inequalities:
   - Capacity cuts
   - Comb inequalities
   - Multi-star inequalities
   - Fractional capacity cuts
3. **Iterative Process**: Add cuts and re-solve until convergence
4. **Lower Bound**: Final LP objective provides lower bound

### Cut Families Implemented

- **Capacity Cuts**: Basic capacity constraints
- **Comb Inequalities**: Advanced valid inequalities
- **Multi-star Cuts**: Generalized multi-commodity cuts
- **Fractional Capacity Cuts**: Strengthened capacity constraints

## Performance Considerations

### Memory Management
- Efficient constraint manager for handling large cut pools
- Dynamic memory allocation for variable-size problems
- Automatic cleanup of unused constraints

### Computational Efficiency
- Optimized separation algorithms
- Parallel-friendly constraint generation
- Release builds recommended for performance

## Troubleshooting

### Common Build Issues

#### Gurobi Not Found
```bash
CMake Error: Could not find Gurobi
```
**Solution**: Set `GUROBI_HOME` environment variable or use `-DGurobi_ROOT=path`

#### Missing C++17 Support
```bash
error: 'std::filesystem' is not available
```
**Solution**: Update compiler to support C++17 (GCC 7+, Clang 5+)

### Runtime Issues

#### License Error
```
Error 10001: No Gurobi license found
```
**Solution**: Ensure Gurobi license is properly installed and accessible

#### Instance Loading Failed
```
Could not load instance: filename.vrp
```
**Solution**: Verify instance file exists in `Instances/` directory and follows TSPLIB format

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/new-cuts`)
3. Commit changes (`git commit -am 'Add new cut separation'`)
4. Push to branch (`git push origin feature/new-cuts`)
5. Create a Pull Request

### Development Guidelines

- Follow existing code style and conventions
- Add appropriate unit tests for new features
- Update documentation for API changes
- Ensure all benchmarks continue to pass

## License

This project incorporates the CVRPSEP library and requires appropriate licensing for Gurobi Optimizer. Please ensure compliance with all applicable licenses.

## References

1. Lysgaard, J., Letchford, A.N., Eglese, R.W. (2004). A new branch-and-cut algorithm for the capacitated vehicle routing problem. Mathematical Programming, 100(2), 423-445.

2. Augerat, P., Belenguer, J.M., Benavent, E., Corberán, A., Naddef, D., Rinaldi, G. (1995). Computational results with a branch and cut code for the capacitated vehicle routing problem. Rapport de recherche RR-949-M, Université Joseph Fourier, Grenoble.

## Support

For issues and questions:
- Open an issue on GitHub
- Check existing issues for similar problems
- Provide minimal reproducible examples