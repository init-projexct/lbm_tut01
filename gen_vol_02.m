% Dimensions
N = 128;
geometry = uint8(ones(N, N, N)); % Start with solid block (1)

% 1. Create the Outer Shell (Hollow out the inside)
% Leaves 1px wall on X and Y boundaries, fully open on Z-ends
geometry(2:N-1, 2:N-1, :) = 0; % 0 = Fluid

% 2. Define the Obstacle Region (The "Plug" with holes)
z_start = 28; % Starting at Z=28 (approx index 27)
z_end = 99;
radius = 12;   % Radius of each cylindrical hole
center = N/2;
offset = radius * 1.8; % Distance between hexagonal centers

% Define the 7 centers (1 middle, 6 surrounding)
centers = [
    center, center;                           % Middle
    center, center + offset;                  % North
    center, center - offset;                  % South
    center + offset*cos(pi/6), center + offset*sin(pi/6); % NE
    center - offset*cos(pi/6), center + offset*sin(pi/6); % NW
    center + offset*cos(pi/6), center - offset*sin(pi/6); % SE
    center - offset*cos(pi/6), center - offset*sin(pi/6); % SW
];

% 3. Apply the Solid "Plug" and Drill the Holes
% First, fill the middle section with solid
geometry(2:N-1, 2:N-1, z_start:z_end) = 1;

% Then, "drill" the 7 holes through that solid section
for i = 1:rows(centers)
    cx = centers(i, 1);
    cy = centers(i, 2);

    for x = 2:N-1
        for y = 2:N-1
            % If within radius of a center, make it fluid
            if sqrt((x-cx)^2 + (y-cy)^2) < radius
                geometry(x, y, z_start:z_end) = 0;
            endif
        endfor
    endfor
endfor

% Write to file
fid = fopen('geometry_128_oct.raw', 'wb');
fwrite(fid, geometry, 'uint8');
fclose(fid);

printf('Hex-packed geometry generated: geometry_128_oct.raw\n');
