% Dimensions
N = 128;
geometry = uint8(ones(N, N, N)); % Start with solid block (1)

% 1. Create the Outer Shell (Hollow out the inside)
geometry(2:N-1, 2:N-1, :) = 0; % 0 = Fluid

% 2. Define the Obstacle Region (The "Plug" with holes)
z_start = 28;
z_end = 99;
radius = 12;
center = N/2;
offset = radius * 1.8;

% Define the 7 centers (1 middle, 6 surrounding)
centers = [
    center, center;
    center, center + offset;
    center, center - offset;
    center + offset*cos(pi/6), center + offset*sin(pi/6);
    center - offset*cos(pi/6), center + offset*sin(pi/6);
    center + offset*cos(pi/6), center - offset*sin(pi/6);
    center - offset*cos(pi/6), center - offset*sin(pi/6);
];

% 3. Apply the Solid "Plug" and Drill the Holes
geometry(2:N-1, 2:N-1, z_start:z_end) = 1;

for i = 1:rows(centers)
    cx = centers(i, 1);
    cy = centers(i, 2);
    for x = 2:N-1
        for y = 2:N-1
            if sqrt((x-cx)^2 + (y-cy)^2) < radius
                geometry(x, y, z_start:z_end) = 0;
            endif
        endfor
    endfor
endfor

% 4. Generate the Surface Mesh
printf('Generating isosurface...\n');
[faces, vertices] = isosurface(double(geometry), 0.5);

% 5. DIRECT BINARY STL WRITER
filename = 'geometry_128_oct.stl';
fid = fopen(filename, 'wb');

% 80-byte header
fwrite(fid, sprintf('%-80s', 'Hex-Packed Plug Binary STL'), 'char');
% Number of facets (uint32)
fwrite(fid, uint32(size(faces, 1)), 'uint32');

printf('Writing Binary STL facets...\n');
for i = 1:size(faces, 1)
    v = vertices(faces(i, :), :);
    n = cross(v(2,:) - v(1,:), v(3,:) - v(1,:));
    if norm(n) > 0, n = n / norm(n); end

    % Use 'float32' as the precision string in fwrite
    fwrite(fid, n, 'float32');
    fwrite(fid, v(1, :), 'float32');
    fwrite(fid, v(2, :), 'float32');
    fwrite(fid, v(3, :), 'float32');
    fwrite(fid, 0, 'uint16');
end

fclose(fid);
printf('Success! Generated: %s (%d facets)\n', filename, size(faces, 1));
