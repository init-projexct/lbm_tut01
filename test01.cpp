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

    global::directories().setOutputDir("./tmp/");

    // 2. Geometry Handling (Using the Sample's Logic)
    pcout << "Reading STL data..." << endl;
    
    // Load the STL with Double precision
    TriangleSet<T> triangleSet("geometry_128_hex.stl", DBL);

    // Center the obstacle manually as seen in the sample
    Cuboid<T> bCuboid = triangleSet.getBoundingCuboid();
    Array<T, 3> obstacleCenter = (T)0.5 * (bCuboid.lowerLeftCorner + bCuboid.upperRightCorner);
    
    triangleSet.translate(-obstacleCenter); // Move to local (0,0,0)
    
    // Position it in the lattice (e.g., 1/4th down the X-axis, center of Y and Z)
    Array<T, 3> centerLB((T)nx/4.0, (T)ny/2.0, (T)nz/2.0);
    triangleSet.translate(centerLB);

    // Create the mesh structures
    plint xDirection = 0;
    plint margin = 1; 
    DEFscaledMesh<T> defMesh(triangleSet, 0, xDirection, margin, Dot3D(0,0,0));
    TriangleBoundary3D<T> boundary(defMesh);

	// 3. Voxelization
    pcout << "Voxelizing the domain..." << endl;
    plint borderWidth = 1;
    plint extendedEnvelopeWidth = 2;
    plint blockSize = 0;      // 0 means use a single block (not sparse)
    plint overlap = 0;        // No overlap between blocks
    bool isPeriodic = false;  // Assume non-periodic for this test
    
    Box3D domain(0, nx-1, 0, ny-1, 0, nz-1);
    
    // This matches the candidate: (boundary, flowType, domain, borderWidth, extendedWidth, blockSize, overlap, isPeriodic)
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
    
    // Set BGK dynamics everywhere
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
    for (plint iT = 0; iT < 1000; ++iT) {
        if (iT % 100 == 0) {
            pcout << "Iteration " << iT << endl;
            pcout << "Average Energy: " << getStoredAverageEnergy(lattice) << endl;
        }
        lattice.collideAndStream();
    }

    pcout << "Simulation complete." << endl;
    return 0;
}
