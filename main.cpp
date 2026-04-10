#include "palabos3D.h"
#include "palabos3D.hh"

// Core dynamics and IO headers for the April 2026 branch
#include "basicDynamics/isoThermalDynamics.h"
#include "basicDynamics/externalForceDynamics.hh" 
#include "latticeBoltzmann/nearestNeighborLattices3D.hh" 
#include "io/serializerIO.h"
#include "io/multiBlockReader3D.h"

using namespace plb;

typedef double T;
#define DESCRIPTOR descriptors::D3Q19Descriptor

int main(int argc, char* argv[]) {
    plbInit(&argc, &argv);
    
    // Set output directory for VTK/DAT files
    global::directories().setOutputDir("./tmp/");

    // Grid dimensions (must match your Python res=128)
    const int nx = 128;
    const int ny = 128;
    const int nz = 128;
    const T omega = 1.0; 

    // Create the lattice
    MultiBlockLattice3D<T, DESCRIPTOR> lattice(nx, ny, nz, 
        new NaiveExternalForceBGKdynamics<T, DESCRIPTOR>(omega));

    // OPTION A: Using 'unsigned char' to match Python's np.uint8 (1 byte per voxel)
    MultiScalarField3D<unsigned char> geometry(nx, ny, nz);
    
    pcout << "Loading geometry_128.raw (1-byte per voxel)..." << std::endl;
    
    // Load the raw binary file directly
    parallelIO::loadRaw("geometry_128.raw", geometry);

    // Define BounceBack (Solid) where the geometry value is 1
    // In your Python script: binary_mask = (smoothed > threshold) 
    // This correctly identifies the solid matrix.
    defineDynamics(lattice, geometry, new BounceBack<T, DESCRIPTOR>(), 1);

    // Initialize the fluid at rest
    initializeAtEquilibrium(lattice, lattice.getBoundingBox(), (T)1.0, Array<T,3>((T)0,(T)0,(T)0));
    lattice.initialize();

    pcout << "Starting Porous Media Simulation..." << std::endl;

    for (plint iT = 0; iT < 1000; ++iT) {
        lattice.collideAndStream();
        
        if (iT % 100 == 0) {
            // pcout ensures only Rank 0 prints, preventing terminal clutter in MPI
            pcout << "Step " << iT 
                  << " | Average Energy: " << getStoredAverageEnergy<T>(lattice) 
                  << std::endl;
        }
    }

    pcout << "Simulation complete. Check ./tmp/ for results." << std::endl;

    return 0;
}
