import numpy as np
from scipy.ndimage import gaussian_filter
import pyvista as pv
import os

def generate_porous_media(size=128, porosity=0.7, scale=5):
    grid = np.random.normal(size=(size, size, size))
    smoothed = gaussian_filter(grid, sigma=scale)
    threshold = np.percentile(smoothed, (1 - porosity) * 100)
    binary_mask = (smoothed > threshold).astype(np.uint8)
    return binary_mask

# --- Settings ---
res = 128 
target_porosity = 0.65
# Save inside the current folder
output_filename = "geometry_128_py.raw"

mask = generate_porous_media(size=res, porosity=target_porosity, scale=4)

# 4. SAVE OUTPUT
mask.tofile(output_filename)

print(f"--- Geometry Generation Complete ---")
print(f"Output File: {os.path.abspath(output_filename)}")
print(f"Actual Porosity: {1.0 - (np.sum(mask) / mask.size):.4f}")
print(f"File Size: {os.path.getsize(output_filename)} bytes")

# 5. 3D PREVIEW
# This will open a window on your Kubuntu desktop
grid = pv.ImageData(dimensions=(res, res, res))
grid.point_data["values"] = mask.flatten(order="F") 
threshed = grid.threshold(0.5) 

plotter = pv.Plotter()
plotter.add_mesh(threshed, color="silver", show_edges=False)
plotter.add_title(f"Porous Volume (Porosity: {target_porosity})")
plotter.show()
