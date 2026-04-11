#include "palabos3D.h"
#include "palabos3D.hh"
#include "basicDynamics/isoThermalDynamics.h"
#include "basicDynamics/externalForceDynamics.hh" 
#include "latticeBoltzmann/nearestNeighborLattices3D.hh" 
#include <fstream>

using namespace plb;

typedef double T;
#define DESCRIPTOR descriptors::D3Q19Descriptor

int main(int argc, char* argv[]) {
    plbInit(&argc, &argv);
    global::directories().setOutputDir("./tmp/");

    const int nx = 128, ny = 128, nz = 128;
    const T omega = 1.0; 

    MultiBlockLattice3D<T, DESCRIPTOR> lattice(nx, ny, nz, 
        new NaiveExternalForceBGKdynamics<T, DESCRIPTOR>(omega));

    MultiScalarField3D<int> geometry(nx, ny, nz);
    
    pcout << "Loading porous_geometry_128.raw..." << std::endl;

    if (global::mpi().isMainProcessor()) {
        std::ifstream inv("porous_geometry_128.raw", std::ios::binary);
        if(!inv) {
            pcout << "Error: File not found!" << std::endl;
            // Simplified exit since .abort() was missing
            exit(-1);
        }

        // We use a Box to iterate through the MultiBlock field properly
        for (int iZ = 0; iZ < nz; ++iZ) {
            for (int iY = 0; iY < ny; ++iY) {
                for (int iX = 0; iX < nx; ++iX) {
                    unsigned char voxelValue;
                    if(inv.read((char*)&voxelValue, sizeof(unsigned char))) {
                        // 'get' is actually the correct one for MultiScalarField3D 
                        // when accessing by global coordinates
                        geometry.get(iX, iY, iZ) = (int)voxelValue;
                    }
                }
            }
        }
        inv.close();
        pcout << "Rank 0: Read complete." << std::endl;
    }
    
    // This is the magic line that actually sends the data to other cores
    geometry.initialize(); 

    pcout << "Geometry synced. Applying physics..." << std::endl;

    defineDynamics(lattice, geometry, new BounceBack<T, DESCRIPTOR>(), 1);
    initializeAtEquilibrium(lattice, lattice.getBoundingBox(), (T)1.0, Array<T,3>((T)0,(T)0,(T)0));
    lattice.initialize();

    pcout << "Simulation starting..." << std::endl;

    for (plint iT = 0; iT < 1000; ++iT) {
        lattice.collideAndStream();
        if (iT % 100 == 0) {
            pcout << "Step " << iT << " | Energy: " << getStoredAverageEnergy<T>(lattice) << std::endl;
        }
    }

    return 0;
}
