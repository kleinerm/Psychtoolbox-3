function rc = moglmorpher(cmd, arg1, arg2, arg3, arg4)
% Matlab OpenGL Morpher - Performs linear morphs between different 3D shapes and
% renders the resulting shape via OpenGL.
%
% The moglmorpher computes linear combinations of shapes and their corresponding
% surface normal vectors and texture coordinate assignments. It then renders the
% resulting shape with its resulting surface normals and texture coordinates in
% an efficient way, using the OpenGL for Matlab (MOGL) functions.
%
% The whole setup for rendering (rigid orientation and position, camera view transforms,
% assignment and setup of textures or material properties, lighting, shaders, ...) is
% left to the calling parent routines, focusing solely on high performance morphing
% and rendering of generic triangle meshes. This allows for maximum flexibility.
%
% For a specific example of usage, have a look at MorphDemo.m
%
%
% Available subcommands and their meaning:
% ----------------------------------------
%
% meshid = moglmorpher('addMesh', obj);
% -- Add a new shape to the collection of shapes to be morphed. 'obj'
% is a single struct that defines the object: Subfields are obj.faces,
% obj.vertices, obj.texcoords, obj.normals. Their meaning is the same as
% the corresponding parameters in the following 'addMesh' subcommand. The
% 'obj' syntax is provided for convenience, as 'obj' in the same format as
% provided by LoadOBJFile, i.e. obj = LoadOBJFile('myfile.obj') will load
% the geometry in 'myfile.obj' into obj, which can then be passed to
% moglmorpher via moglmorpher('addMesh', obj{1}); to add the first mesh
% from 'myfile.obj' into the morpher.
%
% meshid = moglmorpher('addMesh', faces, vertices [, texcoords] [, normals]);
% -- Add a new shape to the collection of shapes to be morphed. faces == Index
% list that defines the topology of the shape: faces is a 3 by n vector. Each of
% the n columns defines one 3D triangle face, the 3 indices in the column are
% indices into the vertices, texcoords and normals vectors that define the
% properties of the vertices.
%
% vertices == A 3-by-m vector that defines the shape of the object: Each of the
% m columns defines the 3D position of one of the corresponding m vertices.
%
% normals == A 3-by-m vector whose single columns define the (nx,ny,nz) components
% of unit normal surface vectors. The normals vector is optional and only needed if
% you want to do proper lighting calculations on your object.
%
% texcoords == A 2-by-m vector of 2D texture coordinates for each corresponding vertex.
% This vector is optional and only needed if you want to apply textures to the object.
%
% The size and dimension of all provided vectors must match (==be identical) for all
% shapes. This is required, because otherwise the linear combination of shapes and
% normal vectors wouldn't be mathematically well defined.
%
% The function call returns a handle (a unique numeric id) for the mesh.
%
% moglmorpher('renderMesh', meshid);
% -- Render the mesh corresponding to the handle 'meshid'.
%
% moglmorpher('renderMorph', weights [,morphnormals=1]);
% -- Compute a linear combination (a weighted average) of all stored meshes, as defined
% by the vector 'weights'. Render the final shape.
% For 'count' shapes, weight is a vector of length 'count'. The i'th scalar entry of weight
% is the coefficient used to integrate the i'th shape into the morph.
%
% moglmorpher('computeMorph', weights [,morphnormals=1]);
% -- Same as 'renderMorph', just that rendering of the morphed shape is
% omitted. You can render the shape later by calling the 'render' subcommand.
%
% finalresult = sum_for_i=1_to_count(shape(i) * weights(i));
% The shape (vertices) and normal vectors are linearly combined. The texture coordinates are
% not altered by the morph. If you set the optional argument morphnormals to zero, then
% normals are not touched by morphing either.
%
% moglmorpher('render');
% -- Renders the last shape again. This is either the last rendered mesh or the last linear
% combination.
%
% glListHandle = moglmorpher('renderToDisplaylist');
% -- Same as subcommand 'render', but the shape is not rendered as an image to the
% framebuffer, but stored to a new OpenGL display list. A unique 'glListHandle' to
% the new list is returned. Using this handle one can render the object
% later on via the command glCallList(glListHandle); and delete it via
% glDeleteLists(glListHandle, 1);
%
% count = moglmorpher('getMeshCount');
% Returns number of stored shapes.
%
% moglmorpher('reset');
% Resets the moglmorpher - deletes all internal data structures.

%
% rc = moglmorpher(cmd, arg1, arg2, arg3, arg4)

% (c) 2006 Mario Kleiner - licensed to you under GPL.

% The GL OpenGL constant definition struct is shared with all other modules:
global GL;

% Internal state of moglmorpher: All vertex/texcoord/normal arrays and such...

% Count of stored keyframe meshes:
persistent objcount;
persistent updatecount;

% Renderbuffers for morphed vertices, normals and texcoords:
persistent vertices;
persistent normals;
persistent texcoords;

% Indexarray for triangles - defines mesh topology:
persistent faces;

% Minimum and maximum vertex index: Used by OpenGL for optimization:
persistent minvertex;
persistent maxvertex;

% Flags to enable/disable use of textures and normals:
persistent usetextures;
persistent usenormals;

% Data type for OpenGL data, GL_FLOAT or GL_DOUBLE.
persistent usetype;

% Arrays which contain all keyshapes (vertices and normals):
persistent keyvertices;
persistent keynormals;

% Sanity check:
if nargin < 1
    help moglmorpher;
    return;
end;

% Initialize MOGL if it didn't happen already in a different module:
if isempty(GL)
    ptbmoglinit;
end;

% Initialize count of keyshapes:
if isempty(objcount)
    objcount = 0;
    usetextures = 0;
    usenormals = 0;
    updatecount = 0;

    % Check if addition is supported for floating-point single precision
    % numbers. If so, we cast all data arrays to single precision (float)
    % and use GL_FLOAT instead of GL_DOUBLE for data-transfer to OpenGL.
    % This is potentially faster for morphing and rendering, but only
    % supported on Matlab-7 or later.
    try
        dummy = single(1)+single(1);
        usetype = GL.FLOAT;
    catch
        usetype = GL.DOUBLE;
    end;
end;

% Subcommand dispatcher:
if strcmp(cmd, 'reset')
    % Reset ourselves. We just clear ourselves out of memory.
    clear moglmorpher;
    return;
end;

if strcmp(cmd, 'getMeshCount')
    % Return count of stored keyshapes:
    rc = objcount;
    return;
end;

if strcmp(cmd, 'addMesh')
    % A new mesh should be added as keyshape to the collection of key-meshes.

    if nargin <2
        error('Need to supply at least an obj mesh object!');
    end;

    argcount = nargin;
    
    % Struct instead of separate variables provided?
    if isstruct(arg1)
        % Yes. Split it up into single ones.
        argcount = 3;
        obj = arg1;
        arg1 = obj.faces;
        arg2 = obj.vertices;
        if ~isempty(obj.texcoords)
            argcount = 4;
            arg3 = obj.texcoords;
        end;
        
        if ~isempty(obj.normals)
            argcount = 5;
            arg4 = obj.normals;
        end;
    end;
    
    % Sanity checks:
    if argcount < 3
        error('Need to supply at least a vector of face indices and vertices!');
    end;

    % If we have multiple keyshapes then all keyshapes need to have the same topology
    % and number/dimensionality of vertices, normals and texcoords, otherwise morphing
    % wouldn't work.
    if objcount>0
        % At least one keymesh is already defined check for consistency:
        if size(faces)~=size(arg1)
            error('Mismatch between size of current faces array and faces array of new mesh!');
        end;
        
        if (size(vertices)~=size(arg2))
            error('Mismatch between size of current vertex coords. array and array of new mesh!');
        end;

        if usetextures & (argcount<4 | isempty(arg3) | (size(texcoords)~=size(arg3)))
            error('Mismatch between size of current texture coords. array and array of new mesh or missing texcoords array!');
        end;
        
        if usenormals & (argcount<5 | isempty(arg4) | (size(normals)~=size(arg4)))
            error('Mismatch between size of current normals array and array of new mesh or missing normals array!');
        end;
    end;
    
    % Cast vector of face indices to unsigned int 32 for use by OpenGL and assign it:
    faces = uint32(arg1);
    minvertex=min(min(faces));
    maxvertex=max(max(faces));
    
    % 2nd argument is vertex array:
    if usetype==GL.FLOAT
        vertices = single(arg2);
    else
        vertices = double(arg2);
    end;

    % 3rd (optional) argument is texture coordinate array:
    if argcount>3 & ~isempty(arg3)
        if usetype==GL.FLOAT
            texcoords = single(arg3);
        else
            texcoords = double(arg3);
        end;
        usetextures = 1;    
    end;
    
    % 4th (optional) argument is normal vectors array:
    if argcount>4 & ~isempty(arg4)
        if usetype==GL.FLOAT
            normals = single(arg4);
        else
            normals = double(arg4);
        end;
        usenormals = 1;
    end;
    
    % Assign vertex array and normals array to new slot in keyshape vector:
    objcount = objcount + 1;
    keyvertices(:,:,objcount)=vertices(:,:);
    keynormals(:,:,objcount)=normals(:,:);

    updatecount = updatecount + 1;

    % Return a handle to the new keymesh:
    rc = objcount;
    return;
end;

if strcmp(cmd, 'renderMesh')
    % A single mesh should be rendered.
    if nargin < 2
        error('You need to supply the handle of the mesh to be rendered!');
    end;
    
    if arg1 <1 | arg1>objcount
        error('Handle for non-existent keyshape provided!');
    end;
    
    % Copy vertices and normals to rendebuffers:
    vertices = keyvertices(:, :, arg1);
    if usenormals
        normals = keynormals(:,:,arg1);
    end;

    updatecount = updatecount + 1;

    % Set command code for rendering current content of renderbuffers:
    cmd = 'render';
end;

if strcmp(cmd, 'renderMorph') | strcmp(cmd, 'computeMorph')
    % A morph (linear combination) of all meshes should be computed and then rendered.
    if nargin < 2
        error('You need to supply the morph-weight vector!');
    end;
    
    if length(arg1) < 1
        error('Morph weight vector needs at least 1 valid entry!');
    end,

    if length(arg1) > objcount
        error('Morph weight vector contains more coefficients than available keyshapes!');
    end;
    
    
    % By default we morph normal vectors as well. As this is not mathematically correct,
    % the parent-code can prevent normal morphing by providing the optional morphnormals=0
    % flag.
    if nargin < 3
        morphnormals=1;
    else
        morphnormals=arg2;
    end;
    
    % Perform morphing of shape and normal vectors:
    vertices(:,:) = keyvertices(:,:,1) * arg1(1);
    for i=2:length(arg1)
        % Add next keyshape:
        vertices(:,:) = vertices(:,:) + (keyvertices(:,:,i) * arg1(i));
    end;

    % Perform morphing of normals as well. This is not strictly correct.
    if usenormals & morphnormals
        normals(:,:) = keynormals(:,:,1) * arg1(1);
        for i=2:length(arg1)
            % Add next keyshape:
            normals(:,:) = normals(:,:) + (keynormals(:,:,i) * arg1(i));
        end;
    end;
    
    updatecount = updatecount + 1;
    
    % Just morph or render as well?
    if strcmp(cmd, 'renderMorph')
        % Set command code for rendering current content of renderbuffers:
        cmd = 'render';
    else
        % Only morphing requested, not rendering. Morph can be rendered via
        % 'rerender' command later.
        return;
    end;
end;

if strcmp(cmd, 'render') | strcmp(cmd, 'renderToDisplaylist')
    % Render current content of renderbuffers via OpenGL:
    if updatecount < 1
        error('Tried to render content of renderbuffers, but renderbuffers not yet filled!');
    end;

    if strcmp(cmd, 'renderToDisplaylist')
        % Create new display list and direct all rendering into it:
        rc = glGenLists(1);
        glNewList(rc, GL.COMPILE);
    else
        % Just render, don't create display list.
        rc = -1;
    end;

    % Enable client-side vertex arrays:
    glEnableClientState(GL.VERTEX_ARRAY);
    % Set pointer to start of vertex array:
    glVertexPointer(size(vertices,1), usetype, 0, vertices);
    
    if usenormals
        % Enable client-side normal vector arrays:
        glEnableClientState(GL.NORMAL_ARRAY);
        % Set pointer to start of normal array:
        glNormalPointer(usetype, 0, normals);
    end;
    
    if usetextures
        % Enable client-side texture coordinate arrays:
        glEnableClientState(GL.TEXTURE_COORD_ARRAY);
        % Set pointer to start of texcoord array:
        glTexCoordPointer(size(texcoords, 1), usetype, 0, texcoords);
    end;

    % Render mesh, using topology given by 'faces':
    if (size(faces,1)==3)
        glDrawRangeElements(GL.TRIANGLES, minvertex, maxvertex, size(faces,2) * 3, GL.UNSIGNED_INT, faces);
    elseif size(faces,1)==4
        glDrawRangeElements(GL.QUADS, minvertex, maxvertex, size(faces,2) * 4, GL.UNSIGNED_INT, faces);        
    else
        error('Invalid number of rows in face index array!');
    end;
        
    % Disable client-side vertex, normals and texcoord arrays:
    glDisableClientState(GL.VERTEX_ARRAY);
    glDisableClientState(GL.NORMAL_ARRAY);
    glDisableClientState(GL.TEXTURE_COORD_ARRAY);

    if rc>-1
        % Finalize our new display list:
        glEndList;
    end;
    
    return;
end;

error('Invalid subcommand specified!');
return;
