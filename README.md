# Lattice Boltzmann Method

This project provides a demonstration of the Lattice Boltzmann Method (LBM) utilizing the Palabos library for fluid dynamics simulation.

## Getting Started

First, clone the repository to your local machine:

```bash
git clone https://github.com/init-projexct/lbm_tut01
```

then run the command
```bash
cd lbm_tut01
mkdir -p build && cd build

# Configure the project using CMake
cmake ..

# Compile using all available CPU cores
make -j$(nproc) 2>&1 | tee log.tx
./main
```

Better run it on PC/server due to CPU core temperature can reach as high as 100°C on laptop
