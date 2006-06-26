function objobject=LoadOBJFile(modelname, debug)
% objobject=LoadOBJFile(modelname, debug)
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

if nargin <1 
  error('You did not provide any filename for the Alias-/Wavefront OBJ file!')  
end;

if nargin <2
    debug = 0;
end;

fid = fopen(modelname,'rt');
if (fid<0)
    error(['Could not open file: ' modelname]);
end;

vnum=1;
f3num=1;
f4num=1;
vtnum=1;
vnnum=1;
gnum=1;
meshcount=1;

Vertices=[];
Faces=Vertices;
Texcoords=Vertices;
Normals=Vertices;
QuadFaces=Vertices;

% Line by line parsing of the obj file
Lyn=fgets(fid);
while Lyn>=0
  s=sscanf(Lyn,'%s',1);
  l=length(Lyn);
  if l==0  % isempty(s) ; 
    if (debug) disp(['empty' Lyn]); end;
  end
    switch s
    case '#' % comment
      if debug disp(Lyn); end;
    case 'v' % vertex
      v=sscanf(Lyn(2:l),'%f');
      Vertices(:,vnum)=v;
      vnum=vnum+1;
    case 'vt'			% textures
      v=sscanf(Lyn(3:l),'%f');
      Texcoords(:,vtnum)=v;
      vtnum=vtnum+1;
    case 'g' % mesh??
      if (debug) disp(Lyn); end;
    case 'usemtl' % what is this??
        if (debug) disp(Lyn); end;
    case 'vn' % normals
      v=sscanf(Lyn(3:l),'%f');
      Normals(:,vnnum)=v;
      vnnum=vnnum+1;
    case 'f' % faces
      Lyn=deblank(Lyn(3:l));
      nvrts=length(findstr(Lyn,' '))+1;
      fstr=findstr(Lyn,'/');
      nslash=length(fstr);
      if nvrts == 3
        if nslash ==3 % vertex and textures
          f1=sscanf(Lyn,'%f/%f');
          f1=f1([1 3 5]);
        elseif nslash==6 % vertex, textures and normals, 
          f1=sscanf(Lyn,'%f/%f/%f');
          f1=f1([1 4 7]);
        elseif nslash==0
          f1=sscanf(Lyn,'%f');
        else
          if (debug) disp(['xyx' Lyn]); end;
          f1=[];
        end
        Faces(:,f3num)=f1;
        f3num=f3num+1;
      elseif nvrts == 4
        if nslash == 4
          f1=sscanf(Lyn,'%f/%f');
          f1=f1([1 3 5 7]);
        elseif nslash == 8
          f1=sscanf(Lyn,'%f/%f/%f');
          f1=f1([1 4 7 10]);
        elseif nslash ==0
          f1=sscanf(Lyn,'%f');
        else
          if (debug) disp(['xx' Lyn]); end;
          f1=[];
        end
        F4(:,f4num)=f1;
        f4num=f4num+1;
      end 
     
    otherwise 
      if ~strcmp(Lyn,char([13 10]))
        if (debug) disp(['OBJ entry unprocessed: ' Lyn]); end;
      end
    end
  
  Lyn=fgets(fid);
end

fclose(fid);

if exist('Faces', 'var')==0
    % No triangles defined. Are there any quads defined?
    if exist('F4','var')
        % Yes. This OBJ defines quads, not triangles. Assign them:
        Faces = F4 - 1;
    else
        % No. Neither triangle- nor quad-definitions! We can't handle this.
        disp('Warning: The OBJ file does not contain any triangle- or quad- polygon definitions!');
        Faces = [];    
    end;
else
    % Take difference in indexing between OpenGL and OBJ into account:
    Faces = Faces - 1;

    % Array with triangle definitions exists. Check for additional quad-definitions:
    if exist('F4','var')
        % Return quad-face definitions in QuadFaces return argument:
        QuadFaces=F4 - 1;
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
return;
