#!/bin/bash

# mach_prep: Machine Preparation Script for Scientific Research & Engineering
# Note: This is an illustrative script. Paths and dependencies must be adapted.

sudo mkdir -p /home/researchx/apps
sudo chown -R $USER:$USER /home/researchx/
mkdir -p /home/researchx/apps/tmp
cd /home/researchx/apps

wget https://github.com/FreeCAD/FreeCAD/releases/download/1.1.0/\
FreeCAD_1.1.0-Linux-x86_64-py311.AppImage
chmod +x FreeCAD_1.1.0-Linux-x86_64-py311.AppImage

sudo dnf install openmpi openmpi-devel vtk vtk-devel paraview -y
echo 'module load mpi/openmpi-x86_64' >> ~/.bashrc && source ~/.bashrc

git clone https://gitlab.com/unigespc/palabos.git
curl -L "https://zenodo.org/records/17899765/files/olb-1.9r0.tgz?download=1" \
-o olb-1.9r0.tgz
tar -zxvf olb-1.9r0.tgz

sed -i 's/parameters_/parameters/g' \
$(find /home/researchx/apps/palabos/src/multiGrid \
-name "multiGridParameterManager.hh")

wget https://www.scilab.org/download/2025.0.0/\
scilab-2025.0.0.bin.linux-x86_64.tar.gz
tar -zxvf scilab-2025.0.0.bin.linux-x86_64.tar.gz

sudo dnf copr enable linuxguy123/openfoam -y && sudo dnf install openfoam -y

sudo dnf install -y goxel leocad librecad solvespace gnuplot R-core \
R-devel ruby ruby-devel ruby-full spyder octave ImageJ \
texstudio texlive-scheme-full gsl-devel tbb-devel

sudo dnf install -y obs-studio obs-studio-plugin-x264 blender geany

wget https://github.com/IfcOpenShell/IfcOpenShell/releases/download/\
v0.8.0/bonsai-0.8.0-py311-linux.zip -O /home/researchx/apps/tmp/bonsai_addon.zip

sudo dnf install -y python3-numpy python3-scipy \
python3-matplotlib python3-pandas python3-pip python3-venv

python3 -m venv /home/researchx/rsch_env
source /home/researchx/rsch_env/bin/activate

python3 -m pip install --upgrade pip
python3 -m pip install pyvista cadquery build123d porespy ifcopenshell

deactivate

sudo dnf install -y nmap vlc mpv cheese v4l-utils kamoso \
dosfstools mtools exfat-utils xsane

sudo dnf install screen htop xfce4-taskmanager xsensors -y

mkdir -p /home/researchx/lbm_tut01/tmp
ls -la /home/researchx/apps
mpirun --version | head -n 1
g++ --version | head -n 1
source /home/researchx/rsch_env/bin/activate && pip list && deactivate
