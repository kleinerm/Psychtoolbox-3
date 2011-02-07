function objobject=LoadOBJFile(modelname, debug, preparse)
% objobject=LoadOBJFile(modelname [, debug] [, preparse])
%
% Load an Alias/Wavefront ASCII-OBJ file and return description of corresponding 3D
% models in 'objobject'. The current implementation will only consider polygons
% with 3 or 4 vertices per polygon, corresponding to OpenGL GL_TRIANGLES or GL_QUADS.
% The routine can only parse ASCII OBJ files, not the (more disk space efficient)
% binary files. It will also ignore any part of the OBJ specification that is not a
% polygon mesh, e.g., NURBS. It will also ignore any kind of .mtl material/texture
% definition files.
%
% Parameters:
% 'modelname' Filename of the OBJ file to read.
% 'debug' (Optional) If set to non-zero, some debug output is written to the Matlab prompt.
% 'preparse' (Optional) If set to non-zero (default), some preparsing is
% done to speed up loading of large OBJ files. Preparsing assumes that all
% vertices, texture coordinates and face indices contain 3 components. If
% loading of your OBJ file fails, retry with preparse==0 to use a more
% generic but slow loader.
%
% Return values:
% 'objobject' objobject is a cell array of structs. For each mesh in the
% OBJ file, a single cell is created in objobject. Each cell contains a
% struct whose subfields contain all information about the mesh. A struct
% consists of the following fields:
%
% faces == 3-by-count or 4-by-count elements index matrix: Each of the 'count' columns
% defines one of 'count' polygons. Each polygon is defined by an integer index into
% the vertices, normals, texcoords arrays. Polygons can be triangles or quadrilaterals.
%
% vertices == A m-by-n vector of vertex position definitions: Each of the n columns
% defines the position of one of n vertices. m Can be 2 for 2D points, 3 for 3D points
% or 4 for 3D points with additional 'w' component.
%
% texcoords == Optional 2-by-n vector of texture coordinates.
%
% normals == Optional 3-by-n vector of surface normals.
%
% If a mesh contains triangle-definitions and quad-definitions, the triangle
% definitions will be returned in 'faces' whereas the Quads will be returned in
% 'quadfaces'. If only one type of primitives is defined, it will always be returned
% in 'faces'. It is possible but uncommon for a OBJ file to not contain 'faces' at all.
%
% Example: Assuming the OBJ file contains exactly one triangle mesh, you'll
% be able to access its data as: objobject{1}.faces --> faces of the mesh,
% objobject{1}.vertices --> vertex definitions, ...
%
% nobjects = length(objobject); Will return the number of meshes in the OBJ
% file in 'nobjects'. objobject{i}.vertices would return the vertex
% definition array of the i'th mesh in the OBJ file.
%
% LIMITATIONS:
% This loader is slooow and may be replaced in the far future by an optimized C-Loader.
%
% This loader is an improved/modified version of the loader from MATLAB-Central, written by:
% W.S. Harwin, University Reading, 2006

% TODO:
% Currently only a single cell is supported: All geometry definition is put
% into index 1 of objobject --> objobject{1}.vertices contains all vertices
% in an OBJ file, objobject{i} for i>1 is always undefined. We need to
% write proper parsing code for such flexibility, but the interface is
% already here, so users will not need to rewrite their code when
% LoadOBJFile is extended in the future.
%
% HISTORY
% 31/03/06, written by Mario Kleiner, derived from W.S. Harwins code.
% 18/09/06, Speedup for common OBJ files due to memory preallocation. (MK)
% 02/09/07, We now handle triangle faces with non-equal vertex/tex/normal indices by remapping to a common index. (MK)
% 10/03/08, Replace deblank() by strtrim() in parser: More robust against leading blanks. (MK)

if nargin<1 
  error('You did not provide any filename for the Alias-/Wavefront OBJ file!')  
end;

if nargin<2
    debug = 0;
end;

if nargin<3
    preparse = 1;
end;

fid = fopen(modelname,'rt');
if (fid<0)
    error(['Could not open file: ' modelname]);
end;

if preparse>0
    % Pre-Parse pass: Load the whole file into a matlab matrix and then count
    % number of vertices et al. to quickly determine the storage requirements.
    preobj = fread(fid, inf, 'uint8=>char')';
    prevnum = length(findstr(preobj, 'v '));
    prevtnum = length(findstr(preobj, 'vt '));
    prevnnum = length(findstr(preobj, 'vn '));
    pref3num = length(findstr(preobj, 'f '));

    % Preallocate output arrays, based on the element counts from the
    % preparse-pass: We may allocate slightly too much, but this should not be
    % a problem, as the real parse pass will correct this.
    Vertices=zeros(3,prevnum);
    Faces=zeros(9,pref3num);
    Texcoords=zeros(3,prevtnum);
    Normals=zeros(3,prevnnum);
    F4=zeros(12,pref3num);
    QuadFaces=[];

    % Rewind to beginning of file in preparation of real data parse pass:
    frewind(fid);
else
    % We do not preallocate, but just create empty arrays. This is needed
    % to accomodate for the special cases where an item has a
    % component-count other than 3, e.g., pure 2D texture coordinates.
    Vertices=[];
    Faces=[];
    Texcoords=[];
    Normals=[];
    QuadFaces=[];
    F4=[];
    prevtnum=1;
end;

% Reset all counts: We recount during real data parse pass to play safe:
vnum=1;
f3num=1;
f4num=1;
vtnum=1;
vnnum=1;
meshcount=1;
totalcount=0;

% Line by line parsing of the obj file
Lyn=fgets(fid);
while Lyn>=0
    s=sscanf(Lyn,'%s',1);
    l=length(Lyn);

    switch s
        case 'f' % faces
            Lyn=strtrim(Lyn(3:l));
            nvrts=length(findstr(Lyn,' '))+1;
            fstr=findstr(Lyn,'/');
            nslash=length(fstr);
            if nvrts == 3
                % Triangle face:
                if nslash ==3 % vertex and textures
                    f1=sscanf(Lyn,'%f/%f');
                    f1=f1([1 3 5 2 4 6 1 3 5]);
                elseif nslash==6 % vertex, textures and normals,
                    f1=sscanf(Lyn,'%f/%f/%f');
                    f1=f1([1 4 7 2 5 8 3 6 9]);
                elseif nslash==0
                    f1=sscanf(Lyn,'%f');
                    f1=f1([1 2 3 1 2 3 1 2 3]);
                else
                    if (debug>1), disp(['xyx' Lyn]); end;
                    f1=[];
                end
                Faces(:,f3num)=f1;
                f3num=f3num+1;
            elseif nvrts == 4
                % Quad face:
                if nslash == 4
                    f1=sscanf(Lyn,'%f/%f');
                    f1=f1([1 3 5 7 2 4 6 8 1 3 5 7]);
                elseif nslash == 8
                    f1=sscanf(Lyn,'%f/%f/%f');
                    f1=f1([1 4 7 10 2 5 8 11 3 6 9 12]);
                elseif nslash ==0
                    f1=sscanf(Lyn,'%f');
                    f1=f1([1 2 3 4 1 2 3 4 1 2 3 4]);
                else
                    if (debug>1)
                        fprintf('Parse error in line %i: Could not process this:\n', totalcount+1);
                        fprintf('%s\n', Lyn);
                        fprintf('nvrts=%i, nslash=%i, f4num=%i\n', nvrts, nslash, f4num);
                        fprintf('Binary representaiton of line is:\n');
                        disp(double(Lyn));
                    end;
                    f1=[];
                end
                F4(:,f4num)=f1; %#ok<AGROW>
                f4num=f4num+1;
            end
        case 'v'  % vertex
            Vertices(:,vnum)=sscanf(Lyn(2:l),'%f');
            vnum=vnum+1;
        case 'vt' % textures
            try
                % Try to assign texture coordinate:
                Texcoords(:,vtnum)=sscanf(Lyn(3:l),'%f');
            catch
                % Failed. Most common reason is that this is not a 3
                % component texture coordinate, so our preallocated array
                % is of wrong size in 1st dimension.
                if vtnum==1
                    % Try to determine real number of components and then
                    % reallocate a proper texture coordinate array:
                    ncomponents = size(sscanf(Lyn(3:l),'%f'),1);
                    Texcoords=zeros(ncomponents, prevtnum);
                    % Restart assignment:
                    Texcoords(:,vtnum)=sscanf(Lyn(3:l),'%f');                    
                else
                    % Failed for some unknown reason. Just throw an error
                    % and abort.
                    psychrethrow(psychlasterror);
                end
            end
            vtnum=vtnum+1;
        case 'vn' % normals
            Normals(:,vnnum)=sscanf(Lyn(3:l),'%f');
            vnnum=vnnum+1;
        case '#'  % comment
            if debug>1 , disp(Lyn); end;
        case 'g'  % mesh.
            if (debug>1), disp(Lyn); end;
        case 'usemtl' % what is this??
            if (debug>1), disp(Lyn); end;
        otherwise
            if ~strcmp(Lyn,char([13 10]))
                if (debug>1), disp(['OBJ entry unprocessed: ' Lyn]); end;
            end
    end

    if debug>0
        % Display progress output:
        totalcount = totalcount + 1;
        if mod(totalcount, 5000)==0
            disp(['LoadOBJFile: Parsing progress: At line ' num2str(totalcount)]);
        end;
    end;

    Lyn=fgets(fid);
end;

fclose(fid);

% Decrement by one: This shall be the true counts:
vnum=vnum - 1;
f3num=f3num - 1;
f4num=f4num - 1;
vtnum=vtnum - 1;
vnnum=vnnum - 1;

if debug > 0
    fprintf('Mesh %s contains:\n', modelname);
    fprintf('Triangles: %i\n', f3num);
    fprintf('Quads: %i\n', f4num);
    fprintf('Vertices: %i\n', vnum);
    fprintf('Texture coordinates: %i\n', vtnum);
    fprintf('Normal vectors: %i\n', vnnum);
end

% Any quads defined?
if f4num > 0
    % Yes. Quads defined in F4: Check if we need to remap texture and
    % normal coordinate indices, just as in the case for triangles below:
    % Do texture coordinates exist?
    if vtnum > 0
        % Yes. Check if face indices for vertices and textures are
        % completely identical:
        idxdiff = sum(abs(F4(1,:) - F4(5,:))) + sum(abs(F4(2,:) - F4(6,:))) + sum(abs(F4(3,:) - F4(7,:))) + sum(abs(F4(4,:) - F4(8,:)));
        if idxdiff~=0
            % Texture indices differ (at least sometimes) from vertex
            % indices. This can't be easily handled by OpenGL, at least not
            % at high performance. We perform manual remapping, permutating
            % the read texture coordinate array, so at the end we can index
            % into the texture array with the same indices as the ones we
            % use for the vertex array. This is more memory intense, but
            % much faster for postprocessing and rendering...

            if debug>0
                fprintf('Inconsistent vertex vs. texture indexing: Remapping...\n');
            end

            SrcTexCoords = Texcoords;
            Texcoords = zeros(size(SrcTexCoords, 1), vnum);

            % Remap/rebuild for each of the f4num faces:
            for i=1:f4num
                Texcoords(:, F4(1,i)) = SrcTexCoords(:, F4(5,i));
                Texcoords(:, F4(2,i)) = SrcTexCoords(:, F4(6,i));
                Texcoords(:, F4(3,i)) = SrcTexCoords(:, F4(7,i));
                Texcoords(:, F4(4,i)) = SrcTexCoords(:, F4(8,i));
            end
        end
    end

    % Do normal coordinates exist?
    if vnnum > 0
        % Yes. Check if face indices for vertices and normals are
        % completely identical:
        idxdiff = sum(abs(F4(1,:) - F4(9,:))) + sum(abs(F4(2,:) - F4(10,:))) + sum(abs(F4(3,:) - F4(11,:))) + sum(abs(F4(4,:) - F4(12,:)));
        if idxdiff~=0
            % Normal indices differ (at least sometimes) from vertex
            % indices. This can't be easily handled by OpenGL, at least not
            % at high performance. We perform manual remapping, permutating
            % the read normals coordinate array, so at the end we can index
            % into the normals array with the same indices as the ones we
            % use for the vertex array. This is more memory intense, but
            % much faster for postprocessing and rendering...

            if debug>0
                fprintf('Inconsistent vertex vs. normals indexing: Remapping...\n');
            end

            SrcNormals = Normals;
            Normals = zeros(size(SrcNormals, 1), vnum);

            % Remap/rebuild for each of the f4num faces:
            for i=1:f4num
                Normals(:, F4(1,i)) = SrcNormals(:, F4(9,i));
                Normals(:, F4(2,i)) = SrcNormals(:, F4(10,i));
                Normals(:, F4(3,i)) = SrcNormals(:, F4(11,i));
                Normals(:, F4(4,i)) = SrcNormals(:, F4(12,i));
            end
        end
    end

    % Strip (now redundant) face indices for textures and normals. Either
    % they were identical from the beginning, or they are now identical
    % after our remap operation:
    F4 = F4(1:4, :);
    
    % Take difference in indexing between OpenGL and OBJ into account.    
    F4 = F4 - 1;
end


if f3num <= 0
    % No triangles defined. Are there any quads defined?
    if exist('F4','var')
        % Yes. This OBJ defines quads, not triangles. Assign them:
        Faces = F4;
    else
        % No. Neither triangle- nor quad-definitions! We can't handle this.
        disp('Warning: The OBJ file does not contain any triangle- or quad- polygon definitions!');
        Faces = [];    
    end;
else
    % Triangles defined in Faces:

    % Do texture coordinates exist?
    if vtnum > 0
        % Yes. Check if face indices for vertices and textures are
        % completely identical:
        idxdiff = sum(abs(Faces(1,:) - Faces(4,:))) + sum(abs(Faces(2,:) - Faces(5,:))) + sum(abs(Faces(3,:) - Faces(6,:)));
        if idxdiff~=0
            % Texture indices differ (at least sometimes) from vertex
            % indices. This can't be easily handled by OpenGL, at least not
            % at high performance. We perform manual remapping, permutating
            % the read texture coordinate array, so at the end we can index
            % into the texture array with the same indices as the ones we
            % use for the vertex array. This is more memory intense, but
            % much faster for postprocessing and rendering...
            
            if debug>0
                fprintf('Inconsistent vertex vs. texture indexing: Remapping...\n');
            end
            
            SrcTexCoords = Texcoords;
            Texcoords = zeros(size(SrcTexCoords, 1), vnum);
            
            % Remap/rebuild for each of the f3num faces:
            for i=1:f3num
                Texcoords(:, Faces(1,i)) = SrcTexCoords(:, Faces(4,i));
                Texcoords(:, Faces(2,i)) = SrcTexCoords(:, Faces(5,i));
                Texcoords(:, Faces(3,i)) = SrcTexCoords(:, Faces(6,i));
            end
        end
    end
    
    % Do normal coordinates exist?
    if vnnum > 0
        % Yes. Check if face indices for vertices and normals are
        % completely identical:
        idxdiff = sum(abs(Faces(1,:) - Faces(7,:))) + sum(abs(Faces(2,:) - Faces(8,:))) + sum(abs(Faces(3,:) - Faces(9,:)));
        if idxdiff~=0
            % Normal indices differ (at least sometimes) from vertex
            % indices. This can't be easily handled by OpenGL, at least not
            % at high performance. We perform manual remapping, permutating
            % the read normals coordinate array, so at the end we can index
            % into the normals array with the same indices as the ones we
            % use for the vertex array. This is more memory intense, but
            % much faster for postprocessing and rendering...

            if debug>0
                fprintf('Inconsistent vertex vs. normals indexing: Remapping...\n');
            end

            SrcNormals = Normals;
            Normals = zeros(size(SrcNormals, 1), vnum);
            
            % Remap/rebuild for each of the f3num faces:
            for i=1:f3num
                Normals(:, Faces(1,i)) = SrcNormals(:, Faces(7,i));
                Normals(:, Faces(2,i)) = SrcNormals(:, Faces(8,i));
                Normals(:, Faces(3,i)) = SrcNormals(:, Faces(9,i));
            end
        end
    end
    
    % Strip (now redundant) face indices for textures and normals. Either
    % they were identical from the beginning, or they are now identical
    % after our remap operation:
    Faces = Faces(1:3, :);

    % Take difference in indexing between OpenGL and OBJ into account.
    Faces = Faces - 1;

    % Array with triangle definitions exists. Check for additional quad-definitions:
    if exist('F4','var')
        % Return quad-face definitions in QuadFaces return argument:
        QuadFaces = F4;
    end;
end;

% Assign variables to proper slot in output-cell-struct:
objobject{meshcount}.faces = Faces;
if exist('QuadFaces', 'var')
    objobject{meshcount}.quadfaces = QuadFaces;
end;
objobject{meshcount}.vertices = Vertices;
objobject{meshcount}.normals = Normals;
objobject{meshcount}.texcoords = Texcoords;

% Done.
if debug > 0
    fprintf('LoadOBJFile: Loading of mesh %s done.\n\n', modelname);
end

return;
