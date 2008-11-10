function obj = AddNormalsToOBJ(obj, flipDir)
% obj = AddNormalsToOBJ(obj [, flipDir=0]);
% Adds surface normal vectors to the vertices of the given input 'obj' 3D
% object representation, returns the new 'obj' with added vertex normal
% vectors. If the input 'obj' already has normal vectors assigned, these
% are overwritten!
%
% The optional parameter 'flipDir' defines if the direction of the computed
% normal vectors should be flipped by 180 degrees. Effectively, it selects
% which side of the triangle is considered to be the "outside" - the direction
% into which the normal vector should point.
%
% The employed algorithm only works on triangles, not on quads. It computes
% a per-triangle normal vector via normalized cross-product computation among
% the edges of the triangle. Then it distributes those normals to the 3
% vertices that define the triangle, accumulating contributions of all
% triangles that share a vertex. At the end, the accumulated normals are
% normalized and assigned as per-vertex normals.

% Child protection:
if nargin < 1
    obj = [];
end

if isempty(obj)
    error('Empty or missing "obj" object argument! You must provide this.');
end

% Assign default CW winding direction, if omitted:
if nargin < 2
    flipDir = [];
end

if isempty(flipDir)
    flipDir = 0;
end

% Map flipDir to a multiplier:
if flipDir > 0
    flipDir = -1;
else
    flipDir = +1;
end

% For each OBJ in the 'obj' struct, perform op:
for i=1:length(obj)
    % Assign current to tmpobj:
    tmpobj = obj{i};

    % Really consists of triangles? We can't handle quads yet:
    if size(tmpobj.faces, 1)~=3
        fprintf('AddNormalsToOBJ: Warning: OBJ subobject %i does not contain triangle faces, but something else, e.g., quads! Cannot handle that. Skipped.\n', i);
        continue;
    end
    
    nrfaces = size(tmpobj.faces, 2);
    if nrfaces < 1
        fprintf('AddNormalsToOBJ: Warning: OBJ subobject %i does not contain any faces! Skipped.\n', i);
        continue;
    end
    
    % (Re-)create per-vertex normals array, initialize it to zero:
    nrvertices = size(tmpobj.vertices, 2);
    tmpobj.normals = zeros(3, nrvertices);
    
    % Create temporary per-normal counter:
    % ccount = zeros(1, size(tmpobj.vertices, 2));
    
    % Iterate over all faces and compute each faces normal:
    for j=1:nrfaces
        % Fetch vertex indices for this triangle. Indices are zero-based,
        % so we need to remap to Matlab/Octave 1-based indexing:
        vidx = tmpobj.faces(:, j) + 1;
        
        % Transfer the three vertices 3D position vectors to vpos:
        vpos = tmpobj.vertices(:, vidx);
        
        % Compute edges in-place, overwriting vpos 2 and 3:
        vpos(:,2) = vpos(:,2) - vpos(:,1);
        vpos(:,3) = vpos(:,3) - vpos(:,1);
        
        % Compute normalized crossproduct: This is the triangle surface normal.
        n = cross(vpos(:,2) , vpos(:,3));
        n = n / norm(n);
        
        % Add it to all contributing vertices for this triangle:
        tmpobj.normals(:, vidx) = tmpobj.normals(:, vidx) + repmat(n, 1, 3);
        % ccount(vidx) = ccount(vidx) + 1;
    end
    
    % All triangles processed: Compute final average normals and flip
    % direction of vector if mandated by flipDir:
    for j=1:nrvertices
        tmpobj.normals(:, j) = tmpobj.normals(:, j) / (norm(tmpobj.normals(:, j)) * flipDir);
    end
    
    % Reassign tmpobj:
    obj{i} = tmpobj;
end
