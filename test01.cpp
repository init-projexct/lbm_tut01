#include "palabos3D.h"
#include "palabos3D.hh"
#include <vector>
#include <iostream>

using namespace plb;
using namespace std;

typedef double T;
#define DESCRIPTOR descriptors::D3Q19Descriptor

int main(int argc, char* argv[]) {
    plbInit(&argc, &argv);

    // 1. Simulation Parameters
    const plint nx = 128;
    const plint ny = 64;
    const plint nz = 64;
    const T omega = 1.0;  
    const T uInlet = 0.05;
    const plint maxIter = 1000;
    const plint saveIter = 250; // Save every 250 steps

    // Set the output directory
    global::directories().setOutputDir("./tmp/");

    // 2. Geometry Handling
    pcout << "Reading STL data..." << endl;
    
    // Using the filename we corrected earlier
    TriangleSet<T> triangleSet("geometry_128_oct.stl", DBL);

    // Center the obstacle manually
    Cuboid<T> bCuboid = triangleSet.getBoundingCuboid();
    Array<T, 3> obstacleCenter = (T)0.5 * (bCuboid.lowerLeftCorner + bCuboid.upperRightCorner);
    
    triangleSet.translate(-obstacleCenter); // Move to local (0,0,0)
    
    // Position in lattice: 1/4th down X-axis, centered in Y and Z
    Array<T, 3> centerLB((T)nx/4.0, (T)ny/2.0, (T)nz/2.0);
    triangleSet.translate(centerLB);

    // Create mesh structures
    plint xDirection = 0;
    plint margin = 1; 
    DEFscaledMesh<T> defMesh(triangleSet, 0, xDirection, margin, Dot3D(0,0,0));
    TriangleBoundary3D<T> boundary(defMesh);

    // 3. Voxelization
    pcout << "Voxelizing the domain..." << endl;
    plint borderWidth = 1;
    plint extendedEnvelopeWidth = 2;
    plint blockSize = 0;      
    plint overlap = 0;        
    bool isPeriodic = false;  
    
    Box3D domain(0, nx-1, 0, ny-1, 0, nz-1);
    
    VoxelizedDomain3D<T> voxelizedDomain(
        boundary, 
        voxelFlag::outside, 
        domain, 
        borderWidth, 
        extendedEnvelopeWidth,
        blockSize,
        overlap,
        isPeriodic
    );
    
    // 4. Lattice Setup
    MultiBlockLattice3D<T, DESCRIPTOR> lattice(voxelizedDomain.getVoxelMatrix());
    
    defineDynamics(lattice, domain, new BGKdynamics<T, DESCRIPTOR>(omega));
    
    // Disable dynamics inside the obstacle
    defineDynamics(lattice, voxelizedDomain.getVoxelMatrix(), domain, 
                   new NoDynamics<T, DESCRIPTOR>(), voxelFlag::inside);

    // 5. Initial and Boundary Conditions
    Array<T, 3> uIn(uInlet, 0.0, 0.0);
    initializeAtEquilibrium(lattice, domain, (T)1.0, uIn);

    lattice.initialize();

    // 6. Main Simulation Loop
    pcout << "Starting simulation..." << endl;
    for (plint iT = 0; iT <= maxIter; ++iT) {
        
        if (iT % 100 == 0) {
            pcout << "Iteration " << iT << " | Average Energy: " << getStoredAverageEnergy(lattice) << endl;
        }

        // --- VTK OUTPUT BLOCK ---
        if (iT % saveIter == 0) {
            pcout << "Writing VTK data at iteration " << iT << "..." << endl;
            
            // This creates files inside the 'tmp' folder
            VtkImageOutput3D<T> vtkOut(createFileName("tmp/data", iT, 6), 1.0);
            vtkOut.writeData<3,float>(*computeVelocity(lattice), "velocity", 1.0);
            vtkOut.writeData<1,float>(*computeDensity(lattice), "density", 1.0);
        }
        // -------------------------

        lattice.collideAndStream();
    }

    pcout << "Simulation complete. Results are in the ./tmp/ directory." << endl;
    return 0;
}
