% Dimensions
N = 128;
geometry = uint8(ones(N, N, N)); % 1 = Solid (Wall)
num_holes = 15;                  % Number of random paths
path_length = N * 1.5;           % How long each path "crawls"
brush_radius = 4;                % Thickness of the holes

% Generate random holes
for h = 1:num_holes
    % Start at a random position on one face (x=1)
    pos = [1, randi(N), randi(N)];
    
    % Direction vector (primarily moving in X, with small Y/Z drift)
    dir = [1, (rand()-0.5), (rand()-0.5)];
    
    for step = 1:path_length
        % Mark the current sphere as fluid (0)
        x_range = max(1, floor(pos(1)-brush_radius)):min(N, ceil(pos(1)+brush_radius));
        y_range = max(1, floor(pos(2)-brush_radius)):min(N, ceil(pos(2)+brush_radius));
        z_range = max(1, floor(pos(3)-brush_radius)):min(N, ceil(pos(3)+brush_radius));
        
        % Local sphere mask
        for i = x_range
            for j = y_range
                for k = z_range
                    if norm([i,j,k] - pos) < brush_radius
                        geometry(i, j, k) = 0; % 0 = Fluid
                    endif
                endfor
            endfor
        endfor
        
        % Update position
        pos = pos + dir;
        
        % Slightly mutate direction for "curviness"
        dir = dir + [(rand()-0.5)*0.1, (rand()-0.5)*0.2, (rand()-0.5)*0.2];
        dir = dir / norm(dir); % Keep speed constant
        
        % Boundary check - stop if we hit a wall
        if any(pos < 1) || any(pos > N), break; endif
    endfor
endfor

% Write to file
fid = fopen('geometry_128_oct.raw', 'wb');
fwrite(fid, geometry, 'uint8');
fclose(fid);

printf('Complex geometry generated: geometry_128_oct.raw\n');
