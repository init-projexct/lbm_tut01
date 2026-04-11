#include "palabos3D.h"
#include "palabos3D.hh"
#include <vector>
#include <iostream>
#include <fstream>

using namespace plb;
using namespace std;

typedef double T;
#define DESCRIPTOR descriptors::D3Q19Descriptor

int main(int argc, char* argv[]) {
    plbInit(&argc, &argv);

    // 1. Parameters & Directory Setup
    const plint nx = 128;
    const plint ny = 64;
    const plint nz = 64;
    const T omega = 1.0;  
    const T uInlet = 0.05;
    const plint maxIter = 2000; // Longer run for stability
    const plint saveIter = 500; 

    global::directories().setOutputDir("./tmp/");
    
    // Prepare the text output file
    string logFile = global::directories().getOutputDir() + "results.csv";
    ofstream csvOut;
    if (global::mpi().isMainProcessor()) {
        csvOut.open(logFile.c_str());
        csvOut << "Iteration,AverageEnergy,MaxVelocity" << endl;
    }

    // 2. Geometry Handling
    pcout << "Reading STL: geometry_128_oct.stl" << endl;
    TriangleSet<T> triangleSet("geometry_128_oct.stl", DBL);

    // Center and position obstacle
    Cuboid<T> bCuboid = triangleSet.getBoundingCuboid();
    Array<T, 3> obstacleCenter = (T)0.5 * (bCuboid.lowerLeftCorner + bCuboid.upperRightCorner);
    triangleSet.translate(-obstacleCenter); 
    triangleSet.translate(Array<T, 3>((T)nx/4.0, (T)ny/2.0, (T)nz/2.0));

    DEFscaledMesh<T> defMesh(triangleSet, 0, 0, 1, Dot3D(0,0,0));
    TriangleBoundary3D<T> boundary(defMesh);

    // 3. Sparse Voxelization (Test 02 Upgrade)
    pcout << "Voxelizing with Sparse Grid (blockSize=32)..." << endl;
    Box3D domain(0, nx-1, 0, ny-1, 0, nz-1);
    
    // Using blockSize=32 for better efficiency on laptop hardware
    VoxelizedDomain3D<T> voxelizedDomain(boundary, voxelFlag::outside, domain, 1, 2, 32);
    
    // 4. Lattice Setup
    MultiBlockLattice3D<T, DESCRIPTOR> lattice(voxelizedDomain.getVoxelMatrix());
    defineDynamics(lattice, domain, new BGKdynamics<T, DESCRIPTOR>(omega));
    defineDynamics(lattice, voxelizedDomain.getVoxelMatrix(), domain, 
                   new NoDynamics<T, DESCRIPTOR>(), voxelFlag::inside);

    // 5. Initial Conditions
    initializeAtEquilibrium(lattice, domain, (T)1.0, Array<T,3>(uInlet, 0.0, 0.0));
    lattice.initialize();

    // 6. Main Simulation Loop
    pcout << "Starting Test 02 Run..." << endl;
    for (plint iT = 0; iT <= maxIter; ++iT) {
        
        if (iT % 100 == 0) {
            T energy = getStoredAverageEnergy(lattice);
            pcout << "Iteration " << iT << " | Energy: " << energy << endl;
            
            // Text Output: Log to CSV
            if (global::mpi().isMainProcessor()) {
                csvOut << iT << "," << energy << "," << "calc_pending" << endl;
            }
        }

        // VTK Output (Corrected template logic)
        if (iT % saveIter == 0) {
            pcout << "Saving VTK snapshot..." << endl;
            VtkImageOutput3D<T> vtkOut(createFileName("tmp/data", iT, 6), 1.0);
            vtkOut.writeData<3,float>(*computeVelocity(lattice), "velocity", 1.0);
            vtkOut.writeData<float>(*computeDensity(lattice), "density", 1.0);
        }

        lattice.collideAndStream();
    }

    if (global::mpi().isMainProcessor()) csvOut.close();
    pcout << "Test 02 Complete. CSV and VTI files generated in ./tmp/" << endl;
    return 0;
}
