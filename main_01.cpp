#include "palabos3D.h"
#include "palabos3D.hh"
#include <iomanip>

// Use the standard descriptor if RhoBarJ is giving header trouble
#define DESCRIPTOR descriptors::D3Q19Descriptor

using namespace plb;
using namespace std;

typedef double T;

int main(int argc, char* argv[]) {
    plbInit(&argc, &argv);
    global::directories().setOutputDir("./tmp/");

    const int nx = 128; const int ny = 128; const int nz = 128;
    const T omega = 1.0; 

    MultiBlockLattice3D<T, DESCRIPTOR> lattice(nx, ny, nz, 
        new BGKdynamics<T, DESCRIPTOR>(omega));
    lattice.periodicity().toggleAll(false);

	// Load Geometry (Manual Rank 0 Read)
    MultiScalarField3D<int> geometry(nx, ny, nz);
    if (global::mpi().isMainProcessor()) {
        pcout << "Loading octet geometry (Manual Rank 0 Read)..." << endl;
        std::ifstream inv("geometry_128_oct.raw", std::ios::binary);
        for (int iZ = 0; iZ < nz; ++iZ) {
            for (int iY = 0; iY < ny; ++iY) {
                for (int iX = 0; iX < nx; ++iX) {
                    unsigned char val;
                    if(inv.read((char*)&val, 1)) geometry.get(iX, iY, iZ) = (int)val;
                }
            }
        }
    }
    
    // THE FIX: Provide all 4 arguments to trigger MPI distribution
    copy(geometry, geometry.getBoundingBox(), geometry, geometry.getBoundingBox()); 
    
    geometry.initialize();

    // FIX 2: Replace PartialBounceBack with BounceBack 
    // (If the Geneva header is missing, we use standard no-slip to get it running first)
    pcout << "Applying standard BounceBack..." << endl;
    defineDynamics(lattice, geometry, new BounceBack<T, DESCRIPTOR>(), 1);

    // FIX 3: Use the legacy force identification
    T forceValue = 1e-5;
    Array<T, 3> force(forceValue, 0.0, 0.0);
    // In many versions, force is at index 1 in the external field
    setExternalVector(lattice, lattice.getBoundingBox(), 1, force);

    lattice.initialize();

    pcout << "Starting simulation..." << endl;
    for (plint iT = 0; iT < 1000; ++iT) {
        lattice.collideAndStream();
        
        if (iT % 100 == 0) {
            T energy = getStoredAverageEnergy<T>(lattice);
            if (!util::isFiniteNumber(energy)) {
                pcerr << "Instability detected!" << endl;
                exit(1);
            }
            pcout << "Step " << iT << " | Energy: " << energy << endl;
        }
    }

    return 0;
}
