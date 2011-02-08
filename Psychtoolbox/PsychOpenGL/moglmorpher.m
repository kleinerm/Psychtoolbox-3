function [rc, varargout] = moglmorpher(cmd, arg1, arg2, arg3, arg4, arg5)
%
% Matlab OpenGL Morpher - Performs linear morphs between different 3D shapes and
% renders the resulting shape via OpenGL. Supports high-performance GPU
% based morphing on recent graphics hardware. Also performs linear morphing
% (linear combinations) between different texture images.
%
% The moglmorpher computes linear combinations of shapes and their corresponding
% surface normal vectors and texture coordinate assignments. It then renders the
% resulting shape with its resulting surface normals and texture coordinates in
% an efficient way, using the OpenGL for Matlab (MOGL) functions.
%
% It also allows to render single sub-meshes efficiently, as well as
% predictions of screen space 2D (x,y) coordinates of vertices in the mesh.
%
% The whole setup for rendering (rigid orientation and position, camera view transforms,
% assignment and setup of textures or material properties, lighting, shaders, ...) is
% left to the calling parent routines, focusing solely on high performance morphing
% and rendering of generic triangle meshes. This allows for maximum flexibility.
%
% For a specific examples of usage, have a look at MorphDemo.m and
% MorphTextureDemo.m.
%
% IMPORTANT: At the end of your script, you must call moglmorpher('reset')
% to release all ressources and reset moglmorpher into a well-defined
% state. Alternatively, you can call 'clear moglmorpher', e.g., if your
% script aborted with an error. If your forget either of these, you'll see
% some weird error messages about "Invalid window record referenced"
% at invocation of the command.
%
%
% Available subcommands and their meaning:
% ----------------------------------------
%
% moglmorpher('ForceGPUMorphingEnabled', enableflag);
%
% Forcefully enable or disable GPU based morphing. 'enableFlag'==1 ->
% Forcefully enable, 'enableFlag'==0 --> Forcefully disable.
%
% moglmorpher can perform all morphing computations and rendering completely
% on the GPU on modern graphics hardware that supports this. This provides
% a significant speedup over morphing in Matlab/Octave code on the slower
% CPU. Normally, moglmorpher auto-detects if GPU based morphing is possible
% and then enables that feature. If GPU morphing is not possible, it
% reverts to a slower Matlab CPU implementation of morphing. For GPU
% morphing, the Psychtoolbox imaging pipeline needs to be enabled by
% passing an optional valid non-zero 'imagingMode' flag to the
% Screen('OpenWindow', ...); call when opening the onscreen window. E.g.,
% the flag 'kPsychNeedFastOffscreenWindows' would enable the pipeline.
%
% There may be cases where either auto-detection goes wrong, or where you
% don't want to use GPU based morphing, e.g., if your hardware/gfx-driver
% is defective and GPU morphing doesn't work correctly, or if you want to
% use the moglmorpher('renderNormals') subfunction which is not yet
% supported in GPU mode, or if you want to perform benchmarking of GPU vs.
% non-GPU mode. In such cases you can use this subfunction to manually
% enable/disable GPU based morphing, overriding the auto-detection code.
%
% It's important to call this function before the first invocation of any
% other subfunction!
%
% GPU based operation should be efficiently supported on all ATI Radeon
% X-1000 or later hardware and all NVidia Geforce-6000 and later hardware.
%
%
%
% All following subfunctions must be only called when at least one onscreen
% window is open!
%
%
% meshid = moglmorpher('addMesh', obj);
% -- Add a new shape to the collection of shapes to be morphed. 'obj'
% is a single struct that defines the object: Subfields are obj.faces,
% obj.vertices, obj.texcoords, obj.normals, obj.colors. Their meaning is
% the same as the corresponding parameters in the following 'addMesh'
% subcommand. The 'obj' syntax is provided for convenience, as 'obj' in the
% same format as provided by LoadOBJFile, i.e. obj =
% LoadOBJFile('myfile.obj') will load the geometry in 'myfile.obj' into
% obj, which can then be passed to moglmorpher via moglmorpher('addMesh',
% obj{1}); to add the first mesh from 'myfile.obj' into the morpher.
%
%
% meshid = moglmorpher('addMesh', faces, vertices [, texcoords] [, normals] [, colors]);
% -- Add a new shape to the collection of shapes to be morphed. faces == Index
% list that defines the topology of the shape: faces is a 3 by n vector. Each of
% the n columns defines one 3D triangle face, the 3 indices in the column are
% indices into the vertices, texcoords, colors and normals vectors that define the
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
% colors == A 3-by-m or 4-by-m vector whose single columns define the
% (red,green,blue [,alpha]) vertex color components of each vertex in
% 'vertices' of unit normal surface vectors. The colors vector is optional
% and only needed if you want to do lighting calculations on your object.
% Most of the time you won't use vertex colors, but instead assign a
% texture for more flexibility and ease of use.
% Note: The current implementation doesn't support morphing of vertex
% colors. Instead it will simply use the vertex 'color' vector of the last
% added mesh for the morphed output -- a fixed assignment of vertex colors.
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
%
% oldEnable = moglmorpher('assumeSparseMorphWeights', enable);
% -- Enable speed optimizations under the assumption that morph weight
% vector contains mostly zero weights, if 'enable' is set to 1. Otherwise
% optimize speed under the assumption of dense weight vectors. The default
% is to assume dense vectors, ie., 'enable' == 0.
%
% Two different algorithms are used, depending on the setting of this
% switch, which have different tradeoffs. However, switching settings here
% is cheap, so you could do it on a per-morph basis. The optimal choice may
% depend on complexity of your keyshape models and speed of your GPU, so
% your mileage will vary and you'll need to benchmark both options for
% optimal speed.
%
%
% moglmorpher('renderMorph', weights [,morphnormals=1]);
% -- Compute a linear combination (a weighted average) of all stored meshes, as defined
% by the vector 'weights'. Render the final shape.
% For 'count' shapes, weight is a vector of length 'count'. The i'th scalar entry of weight
% is the coefficient used to integrate the i'th shape into the morph.
%
%
% moglmorpher('computeMorph', weights [,morphnormals=1]);
% -- Same as 'renderMorph', just that rendering of the morphed shape is
% omitted. You can render the shape later by calling the 'render' subcommand.
%
% finalresult = sum_for_i=1_to_count(shape(i) * weights(i));
% The shape (vertices) and normal vectors are linearly combined. The texture coordinates are
% not altered by the morph, neither are the vertex colors. If you set the
% optional argument morphnormals to zero, then normals are not touched by
% morphing either.
%
%
% moglmorpher('render');
% -- Renders the last shape again. This is either the last rendered mesh or the last linear
% combination.
%
%
% glListHandle = moglmorpher('renderToDisplaylist');
% -- Same as subcommand 'render', but the shape is not rendered as an image to the
% framebuffer, but stored to a new OpenGL display list. A unique 'glListHandle' to
% the new list is returned. Using this handle one can render the object
% later on via the command glCallList(glListHandle); and delete it via
% glDeleteLists(glListHandle, 1);
%
%
% moglmorpher('renderNormals' [,normalLength=1]);
% -- Renders the surface normal vectors of the last shape in green, either at unit-length,
% or at 'normalLength' if this argument is provided. This is a helper function for
% checking the correctness of computed normals. It is very slow!
%
%
% vpos = moglmorpher('getVertexPositions', windowPtr [, startidx=1] [, endidx]);
% -- Compute and return a matrix which contains the projected screen space coordinates of all
% vertices that would be rendered when calling moglmorpher('render'). windowPtr is the handle
% of the window into which we render. Optional arguments startidx and endidx define the
% index of the first vertex, resp. the last vertex to transform. The returned 'vpos' is a
% vcount-by-3 matrix, where vcount is the number of returned vertices, and row i contains the
% projected 3D position of the i'th vertex vcount(i,:) = (screen_x, screen_y, depth_z);
%
%
% count = moglmorpher('getMeshCount');
% -- Returns number of stored shapes.
%
%
% textureCoordinates = moglmorpher('getTexCoords');
% -- Returns current vector of textureCoordinates as used for rendering
% meshes.
%
%
% [vertices, normals] = moglmorpher('getGeometry');
% -- Returns current vector of 'vertices' and (optionally) normals as used
% for rendering meshes. Call this after (at least one call to)
% 'computeMorph' or 'renderMorph' to retrieve the current morphed mesh.
% vertices and normals are 3-by-n matrices, each column encoding the three
% components (x,y,z) of a single 3D vertex position or vertex normal.
%
%
% [texid, gltexid, gltextarget] = moglmorpher('morphTexture', windowPtr, morphWeights, keyTextures);
% -- Compute a linear combination of the Psychtoolbox textures stored in
% vector 'keyTextures', using the values in vector 'morphWeights' as
% weights. Return handles to the computed (morphed) texture. 'texid' is a
% Psychtoolbox texture handle (e.g., for use with Screen('DrawTexture')),
% gltexid is an OpenGL texture handle and gltextarget is an OpenGL texture
% target. The texture could be used for rendering, e.g., onto a surface via
% glBindTexture(gltextarget, gltexid); and disabled again via
% glBindTexture(gltextarget, 0);
%
% Input textures must be rectangle textures and the output texture will be
% a rectangle texture. The morphed texture 'texid' is owned by moglmorpher.
% You must not close it or bad things will happen!
%
%
% moglmorpher('reset');
% -- Resets the moglmorpher - deletes all internal data structures.
%
%

% (c) 2006 Mario Kleiner - licensed to you under GPL.
%
% History:
% Sometimes 2006 - Written (MK).
%
% 10.09.2007  Added code for high-performance GPU based morhping. (MK).
%             This makes use of FBOs, PBOs and VBOs plus the PTB imaging
%             pipeline to perform all morphing on the GPU, ie., all data
%             always resides in VRAM, only minimal involvement of the CPU
%             and host<->GPU bus --> Should provide significant speedups
%             for large meshes or many meshes. Observed speedup on
%             MacBookPro with ATI Radeon X1600 over Matlab7.4 is 3x.
%
% 03.11.2010  Add function 'getGeometry' to retrieve current (morphed) mesh
%             geometry. (MK).
%
% 03.11.2010  Add function 'morphTextue' to morph textures as well (MK).
%
% 03.11.2010  Add support for static vertex colors (non-morphable for now) (MK).
%

% The GL OpenGL constant definition struct is shared with all other modules:
global GL;

% Internal state of moglmorpher: All vertex/texcoord/normal arrays and such...

% Count of stored keyframe meshes:
persistent objcount;
persistent updatecount;
persistent resynccount;

% Renderbuffers for morphed vertices, normals and texcoords:
persistent vertices;
persistent normals;
persistent texcoords;
persistent vertcolors;

% Indexarray for triangles - defines mesh topology:
persistent faces;

% Minimum and maximum vertex index: Used by OpenGL for optimization:
persistent minvertex;
persistent maxvertex;

% Flags to enable/disable use of textures and normals:
persistent usetextures;
persistent usenormals;
persistent usecolors;

% Data type for OpenGL data, GL_FLOAT or GL_DOUBLE.
persistent usetype;

% Arrays which contain all keyshapes (vertices and normals):
persistent keyvertices;
persistent keynormals;
persistent realVertexCount;
persistent realNormalCount;

% glDrawRangeElements() command supported?
persistent drawrangeelements;

% Pointer to and size of current memory feedback buffer:
persistent feedbackptr;
persistent feedbacksize;

% Internal state needed for fully GPU based implementation:
persistent win;
persistent gpubasedmorphing;
persistent keyshapes;
persistent masterkeyshapetex;
persistent morphbuffer;
persistent vbo;
persistent colorvbo;
persistent ibo;

persistent vbovertexstart;
persistent vbonormalstart;

persistent morphshader;
persistent morphOperator;
persistent morphOperatorNoNormals;
persistent shadermorphweight;

persistent multimorphshader;
persistent multishadermorphcount;
persistent multimorphOperator;
persistent multimorphOperatorNoNormals;

persistent weighttex;
persistent weighttexgl;
persistent oldWeightLength;

persistent isbuggyatidriver;
persistent useSparseMorph;

% Texture morphing:
persistent texmorphbuffer;
persistent texmorphOperator;
persistent texmorphshader;
persistent texshadermorphweight;

% Sanity check:
if nargin < 1
    help moglmorpher;
    return;
end;

% Special subcommand: The only one to be called without open onscreen
% window:
if strcmpi(cmd, 'ForceGPUMorphingEnabled')
    if nargin < 2
        error('You must supply the enable flag for subfunction ForceGPUMorphingEnabled!');
    end;

    if objcount > 0
        error('You must call subfunction "ForceGPUMorphingEnabled" before the very first call to subfunction "addMesh"!');
    end
    
    % Assign override flag:
    gpubasedmorphing = arg1;
    
    if gpubasedmorphing
        fprintf('moglmorpher: INFO: Fully GPU based morphing forcefully enabled by usercode!');
    else
        fprintf('moglmorpher: INFO: Fully GPU based morphing forcefully disabled by usercode!');
    end

    rc = 0;   
   return;
end

% Initialize MOGL if it didn't happen already in a different module:
if isempty(GL)
    InitializeMatlabOpenGL;
end;

% Initialize feedback memory pointers:
if isempty(feedbackptr)
   feedbackptr  = 0;
   feedbacksize = 0; 
end;

if isempty(win)
    % Guess-o-matic: We need the window handle of the onscreen window in
    % which we are supposed to morph & render, but this doesn't get passed
    % by default. Try to auto-detect:
    win = Screen('GetOpenGLDrawMode');
    if win == 0
        % No window bound as drawing target. Just choose first open
        % onscreen window:
        windows = Screen('Windows');
        if isempty(windows)
            % Tried to invoke a moglmorpher subfunction without any open
            % onscreen windows. That's a no-no...
            error('You must open at least one onscreen window before calling any moglmorpher() subfunctions!');
        end
        win = windows(1);
    end
end

% Check if our renderer is an ATI Radeon X-1600 under MacOS/X. If so, set
% the buggy renderer flag to take some bug of that gpu or its driver on
% OS/X into account...
if isempty(isbuggyatidriver)
    % Prior assumption: No buggy driver...
    isbuggyatidriver = 0;
    
    % ATI X1000 series on OS/X?
    if ~isempty(findstr(glGetString(GL.RENDERER), 'Radeon X1')) && IsOSX
        % OS/X 10.4.x ATI driver is buggy, but not 10.5.5 and later.
        compinfo = Screen('Computer');
        osrelease = sscanf(compinfo.system, '%*s %*s %*i.%i.%i');
        if (osrelease(1) < 5) || ((osrelease(1) == 5) && (osrelease(2) < 5))
            % Buggy 10.4.x or a 10.5.4 or earlier: Enable workaround.
            isbuggyatidriver = 1;
        end
    end
end

if isempty(gpubasedmorphing)
    % Test if fully GPU based morphing via FBO's VBO's PBO's and shaders is
    % possible. Use it, if so:
    gpubasedmorphing = 0;

    oldWeightLength = -1;

    % Check if all OpenGL extensions for GPU based morphing are available:
    if ~isempty(findstr(glGetString(GL.EXTENSIONS), '_vertex_buffer_object')) && ...
            ~isempty(findstr(glGetString(GL.EXTENSIONS), '_pixel_buffer_object')) && ...
            ~isempty(findstr(glGetString(GL.EXTENSIONS), '_framebuffer_object')) && ...
            ~isempty(findstr(glGetString(GL.EXTENSIONS), 'GL_ARB_shading_language')) && ...
            ~isempty(findstr(glGetString(GL.EXTENSIONS), 'GL_ARB_shader_objects')) && ...
            ~isempty(findstr(glGetString(GL.EXTENSIONS), 'GL_ARB_fragment_shader')) && ...
            (~isempty(findstr(glGetString(GL.EXTENSIONS), 'GL_APPLE_float_pixels')) || ...
             ~isempty(findstr(glGetString(GL.EXTENSIONS), '_color_buffer_float')))
        % Test passed: All required extensions are supported.
        
        % Check if at least minimal imaging pipeline is active. Otherwise
        % this won't work:
        
        [targetwindow, IsOpenGLRendering] = Screen('GetOpenGLDrawMode');
        if IsOpenGLRendering
            % Disable OpenGL mode:
            Screen('EndOpenGL', targetwindow);
        end

        winfo=Screen('GetWindowInfo', win);
        
        if IsOpenGLRendering
            % Reenable OpenGL mode:
            Screen('BeginOpenGL', targetwindow);
        end

        if (bitand(winfo.ImagingMode, kPsychNeedFastBackingStore)==0) && (winfo.ImagingMode~= kPsychNeedFastOffscreenWindows)
            fprintf('\n\nmoglmorpher: HINT: Your hardware supports fast GPU based morphing, but i cannot use it.\n');
            fprintf('moglmorpher: HINT: Please set the optional "imagingMode" flag of Screen(''OpenWindow'', ...)\n');
            fprintf('moglmorpher: HINT: at least equal to "kPsychNeedFastOffscreenWindows" in order to enable fast mode.\n\n');
        else
            % Switch to fully GPU based morphing and rendering. All work done by GPU, CPU
            % will be mostly idle:
            gpubasedmorphing = 1;
            fprintf('moglmorpher: INFO: Fully GPU based Morphing & Rendering enabled!\n');
        end
    else
        fprintf('moglmorpher: INFO: Only using CPU based Morphing.\n');
    end
end

% Initialize count of keyshapes:
if isempty(objcount)
    objcount = 0;
    usetextures = 0;
    usenormals = 0;
    usecolors = 0;
    updatecount = 0;
    resynccount = 0;
    useSparseMorph = 0;
    realVertexCount = 0;
    realNormalCount = 0;
    
    % Check if addition is supported for floating-point single precision
    % numbers. If so, we cast all data arrays to single precision (float)
    % and use GL_FLOAT instead of GL_DOUBLE for data-transfer to OpenGL.
    % This is potentially faster for morphing and rendering, but only
    % supported on Matlab-7 or later.
    try
        dummy = moglsingle(1)+moglsingle(1);
        if dummy == 2
           usetype = GL.FLOAT;
        else
           usetype = GL.DOUBLE;
        end
    catch
        usetype = GL.DOUBLE;
    end;
    
    % Override: For GPU based morphing, we won't do the math in
    % Matlab/Octave, so we can always use the more efficient GL.FLOAT type:
    if gpubasedmorphing
        usetype = GL.FLOAT;
    end
end;

% Check if hardware supports glDrawRangeElements():
if isempty(drawrangeelements)
    if isempty(findstr(glGetString(GL.EXTENSIONS), 'GL_EXT_draw_range_elements'))
        % No glDrawRangeElements() support.
        drawrangeelements = 0;
    else
        % glDrawRangeElements() supported.
        drawrangeelements = 1;
    end;
end;

% Subcommand dispatcher:
if strcmpi(cmd, 'reset')
   % Reset ourselves:
   
   if (gpubasedmorphing > 0) && (objcount > 0)
       % Query current OpenGL state:
       [targetwindow, IsOpenGLRendering] = Screen('GetOpenGLDrawMode');

       if IsOpenGLRendering
           % Disable OpenGL mode:
           Screen('EndOpenGL', targetwindow);
       end
       
       % Release keyshape textures with vertices and normals:
       for i=1:objcount
           Screen('Close', keyshapes(i));
       end
       keyshapes = [];
       
       % Reset masterkeyshape texture, if any:
       if ~isempty(masterkeyshapetex)
           Screen('Close', masterkeyshapetex);
           masterkeyshapetex = [];
       end

       if ~isempty(weighttex)
           % Release weight texture:
           Screen('Close', weighttex);
           weighttex = [];
       end

       % Release Offscreen windows:
       Screen('Close', morphbuffer(1));
       Screen('Close', morphbuffer(2));
       morphbuffer = [];
       
       % Release shader:
       Screen('Close', morphOperator);
       Screen('Close', morphOperatorNoNormals);
       morphOperatorNoNormals = [];
       morphOperator = [];
       
       Screen('Close', multimorphOperator);
       Screen('Close', multimorphOperatorNoNormals);
       multimorphOperatorNoNormals = [];
       multimorphOperator = [];
       
       glDeleteProgram(morphshader);
       morphshader = [];
       
       glDeleteProgram(multimorphshader);
       multimorphshader = [];
       
       if IsOpenGLRendering
           % Reenable OpenGL mode:
           Screen('BeginOpenGL', targetwindow);
       end
       
       % Release VBOs:
       glDeleteBuffers(1, vbo);
       vbo = [];
       vbovertexstart = [];
       vbonormalstart = [];
       
       glDeleteBuffers(1, ibo);
       ibo = [];
       
       if ~isempty(colorvbo)
           glDeleteBuffers(1, colorvbo);
           colorvbo = [];
       end
       
       weighttexgl = [];
       oldWeightLength = -1;
       
       gpubasedmorphing = [];
   end
   
   % Release memory buffer, if any:
   if feedbackptr~=0
      moglfree(feedbackptr);
      feedbackptr=0;
      feedbacksize=0;
   end;
   
   % Release everything else. We can not use the clear
   % function, because its use inside function bodies
   % is forbidden under Octave.
   objcount = [];
   drawrangeelements = [];
   vertices = [];
   normals = [];
   texcoords = [];
   vertcolors = [];
   faces = [];
   keyvertices = [];
   keynormals = [];
   win = [];
   useSparseMorph = [];
   realVertexCount = [];
   realNormalCount = [];
   resynccount = [];
   
   if ~isempty(texmorphbuffer)
       Screen('Close', texmorphbuffer);
       texmorphbuffer = [];
   end

   if ~isempty(texmorphOperator)
       Screen('Close', texmorphOperator);
       texmorphOperator = [];
   end

   if ~isempty(texmorphshader)
       glDeleteProgram(texmorphshader);
       texmorphshader = [];
       texshadermorphweight = [];
   end
   
   rc = 0;
   
   return;
end;

if strcmpi(cmd, 'getMeshCount')
    % Return count of stored keyshapes:
    rc = objcount;
    return;
end;

if strcmpi(cmd, 'getTexCoords')
    % Return current internal texcoords array:
    rc = texcoords;
    return;
end

if strcmpi(cmd, 'assumeSparseMorphWeights')
    if nargin < 2
        error('Need to supply "enable" flag with value 0 or 1!');
    end;

    % Return old setting:
    rc = useSparseMorph;

    % Assign new setting:
    useSparseMorph = arg1;
    
    % Done.
    return;
end

if strcmpi(cmd, 'addMesh')
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

        if isfield(obj, 'colors') && ~isempty(obj.colors)
            argcount = 6;
            arg5 = obj.colors;
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

        if usetextures && (argcount<4 || isempty(arg3) || any(size(texcoords)~=size(arg3)))
            error('Mismatch between size of current texture coords. array and array of new mesh or missing texcoords array!');
        end;
        
        if usenormals && (argcount<5 || isempty(arg4) || any(size(normals)~=size(arg4)))
            error('Mismatch between size of current normals array and array of new mesh or missing normals array!');
        end;

        if usecolors && (argcount<6 || isempty(arg5) || any(size(vertcolors)~=size(arg5)))
            error('Mismatch between size of current vertex colors array and array of new mesh or missing colors array!');
        end;
    end;
        
    % Cast vector of face indices to unsigned int 32 for use by OpenGL and assign it:
    faces = uint32(arg1);
    minvertex=min(min(faces));
    maxvertex=max(max(faces));
    
    % 2nd argument is vertex array:
    vertices = double(arg2);
    
    % 3rd (optional) argument is texture coordinate array:
    if argcount>3 && ~isempty(arg3)
        texcoords = double(arg3);
        usetextures = 1;    
    end;
    
    % 4th (optional) argument is normal vectors array:
    if argcount>4 && ~isempty(arg4)
        normals = double(arg4);
        usenormals = 1;
    end;

    % Store true size of arrays:
    realVertexCount = size(vertices, 2);
    if usenormals
        realNormalCount = size(normals, 2);
    else
        realNormalCount = 0;
    end
    
    % 5th (optional) argument is vertex colors array:
    if argcount>5 && ~isempty(arg5)
        vertcolors = double(arg5);
        usecolors = 1;
    end;
    
    
    % Assign vertex array and normals array to new slot in keyshape vector:
    objcount = objcount + 1;
    
    if ~gpubasedmorphing
        % CPU based operation: Just copy vertices and normals into our
        % internal arrays of keyshapes:
        
        % If runtime can handle native single() aka FLOAT data type, then
        % downcase to float --> More memory and speed efficient on CPU!
        % Otherwise (Matlab < 7.0 or current Octave 2 and 3) keep it at our
        % current double() format:
        if usetype==GL.FLOAT
            vertices = moglsingle(vertices);
            texcoords = moglsingle(texcoords);
            normals = moglsingle(normals);
            vertcolors = moglsingle(vertcolors);
        end;
        
        keyvertices(:,:,objcount)=vertices(:,:);
        keynormals(:,:,objcount)=normals(:,:);
    else    
        % GPU based operation: Setup "shape textures" ie, floating point
        % textures that encode vertex and normal vectors.

        % The current format of vertices, texcoords and normals is
        % double(), which is what we need for creation of floating point
        % textures...
        
        % Build shape vector texture:
        texlinelength = 1024;
        nrows = ceil(size(vertices, 2) / texlinelength);
        ncols = texlinelength;
        npad = (texlinelength - mod(size(vertices, 2), texlinelength));

        if npad ~= texlinelength
            invertices = [vertices zeros(3, npad)];
            if usenormals
                innormals = [normals zeros(3, npad)];
            end
        else
            invertices = vertices;
            if usenormals
                innormals = normals;
            end
        end

        c1 = flipud(transpose(reshape(invertices(1,:), ncols, nrows)));
        c2 = flipud(transpose(reshape(invertices(2,:), ncols, nrows)));
        c3 = flipud(transpose(reshape(invertices(3,:), ncols, nrows)));

        if usenormals
            c1 = [ flipud(transpose(reshape(innormals(1,:), ncols, nrows))) ; c1 ];
            c2 = [ flipud(transpose(reshape(innormals(2,:), ncols, nrows))) ; c2 ];
            c3 = [ flipud(transpose(reshape(innormals(3,:), ncols, nrows))) ; c3 ];
            nrows = nrows * 2;
        end
        
        myshapeimg = zeros(nrows,ncols,3);
        myshapeimg(:,:,1) = c1;
        myshapeimg(:,:,2) = c2;
        myshapeimg(:,:,3) = c3;
        
        % Query current OpenGL state:
        [targetwindow, IsOpenGLRendering] = Screen('GetOpenGLDrawMode');

        if IsOpenGLRendering
            % Disable OpenGL mode:
            Screen('EndOpenGL', targetwindow);
        end
        
        % Convert "keyshapematrix" with encoded vertex- and normal vectors
        % into a 32bpc floating point texture (floatprecision == 2) and
        % request immediate conversion into optimal storage format for
        % Screen('TransformTexture') (orientation == 1):
        keyshapes(objcount) = Screen('MakeTexture', win, myshapeimg, [], [], 2, 1);
           
        % Reset masterkeyshape texture, if any:
        if ~isempty(masterkeyshapetex)
            Screen('Close', masterkeyshapetex);
            masterkeyshapetex = [];
        end

        % First time invocation?
        if objcount == 1
            % Yes! Need to allocate and setup FBO's PBO's and VBO's and create
            % GLSL operator for morphing on GPU:

            % Creation and init of special GLSL morph-operator for shape morphing
            % on GPU:
            morphshader = LoadGLSLProgramFromFiles('LinearCombinationOfTwoImagesShader.frag.txt');
            morphOperator = CreateGLOperator(win, kPsychNeed32BPCFloat, morphshader, 'Multiply-Accumulate operator for shape morphing');
            morphOperatorNoNormals = CreateGLOperator(win, kPsychNeed32BPCFloat, morphshader, 'No normals Multiply-Accumulate operator for shape morphing');
            if usenormals
                Screen('HookFunction', morphOperatorNoNormals, 'PrependBuiltin', 'UserDefinedBlit', 'Builtin:RestrictToScissorROI', sprintf('%i:%i:%i:%i', 0, 0, ncols, nrows/2));
            end
            
            % Need 'shadermorphweight' uniform location, so we can assign a
            % different weight for morphing of each keyshape in morph-loop:
            shadermorphweight = glGetUniformLocation(morphshader, 'Image1Weight');

            % Setup all other uniforms to their fixed values:
            glUseProgram(morphshader);

            % Don't need 2nd weight of this shader for our purpose: Set it
            % to 1.0 so we get a nice multiply-accumulate behaviour:
            glUniform1f(glGetUniformLocation(morphshader, 'Image2Weight'), 1.0);

            % Assign texture units to samplers:
            glUniform1i(glGetUniformLocation(morphshader, 'Image1'), 0);
            glUniform1i(glGetUniformLocation(morphshader, 'Image2'), 1);
            glUseProgram(0);

            multimorphshader = LoadGLSLProgramFromFiles('MultiMorphShader.frag.txt');
            multimorphOperator = CreateGLOperator(win, kPsychNeed32BPCFloat, multimorphshader, 'Singlepass Multiply-Accumulate operator for shape morphing');
            multimorphOperatorNoNormals = CreateGLOperator(win, kPsychNeed32BPCFloat, multimorphshader, 'No normals Singlepass Multiply-Accumulate operator for shape morphing');
            if usenormals
                Screen('HookFunction', multimorphOperatorNoNormals, 'PrependBuiltin', 'UserDefinedBlit', 'Builtin:RestrictToScissorROI', sprintf('%i:%i:%i:%i', 0, 0, ncols, nrows/2));
            end

            % Query needed locations of shader uniforms:
            multishadermorphcount = glGetUniformLocation(multimorphshader, 'Count');
            multishadermorphstride = glGetUniformLocation(multimorphshader, 'Stride');

            % Setup all other uniforms to their fixed values:
            glUseProgram(multimorphshader);

            % Assign texture unit to sampler:
            glUniform1i(glGetUniformLocation(multimorphshader, 'Image'), 0);
            glUniform1i(glGetUniformLocation(multimorphshader, 'WeightImage'), 1);
            glUniform1i(multishadermorphstride, nrows);
            glUseProgram(0);

            % Create offscreen floating point windows as morph-accumulation
            % buffers: Need two of them for buffer-pingpong...
            % We request 128 bpp == 32 bpc float precision:
            morphbuffer(1) = Screen('OpenOffscreenWindow', win, [0 0 0 0], [0 0 ncols nrows], 128);
            morphbuffer(2) = Screen('OpenOffscreenWindow', win, [0 0 0 0], [0 0 ncols nrows], 128);

            % Create and setup PBO-backed VBO:
            Screen('BeginOpenGL', win);

            vbo = glGenBuffers(1);
            glBindBuffer(GL.ARRAY_BUFFER, vbo);

            % Calculate size of VBO in bytes:
            arraycount = 1;
            if usetextures
                arraycount = arraycount + 1;
            end

            if usenormals
                arraycount = arraycount + 1;
                nrows= nrows / 2;
            end

            buffersize = arraycount * ncols*nrows*4*4;
            
            if ~usetextures
                % Allocate but don't initialize it, ie NULL pointer == 0    
                glBufferData(GL.ARRAY_BUFFER, buffersize, 0, GL.STREAM_COPY);
                vbovertexstart = 0;
            else
                % Allocate it and initialize its start with texture
                % coordinates:

                % Assume storage requirement for texcoords is nr-components *
                % nr-coords * sizeof(float) (ie 4)
                texcoordsize = size(texcoords,1) * size(texcoords,2) * 4;
                glBufferData(GL.ARRAY_BUFFER, buffersize, 0, GL.STREAM_COPY);
                glBufferSubData(GL.ARRAY_BUFFER, 0, texcoordsize, moglsingle(texcoords));
                vbovertexstart = texcoordsize;
            end
            
            if usenormals
                vbonormalstart = vbovertexstart + (4 * size(invertices, 2) * 4);
            else
                vbonormalstart = 0;
            end
            
            glBindBuffer(GL.ARRAY_BUFFER, 0);
            
            % Setup another VBO for the polygon face indices:
            ibo = glGenBuffers(1);
            glBindBuffer(GL.ELEMENT_ARRAY_BUFFER_ARB, ibo);
            % Allocate buffer for number of face indices stored in 'faces',
            % each taking up 4 Bytes (== sizeof(uint32)) of memory.
            % Initialize immediately with content of 'faces' array and tell
            % OpenGL that this won't change at all during operation
            % (STATIC_DRAW):
            glBufferData(GL.ELEMENT_ARRAY_BUFFER_ARB, size(faces,1) * size(faces,2) * 4, faces, GL.STATIC_DRAW);
            glBindBuffer(GL.ELEMENT_ARRAY_BUFFER_ARB, 0);

            Screen('EndOpenGL', win);
        end

        if usenormals
            % Ok, a bit strange and inefficient maybe, but needed for
            % correct semantics/compatibility to non-GPU-morph case. We
            % need to init the normals-array subsection of the VBO with the
            % normals of the current shape. In case the master-script
            % requests to not morph normal vectors, this will guarantee
            % that the normal vectors of the last added mesh will be used
            % instead of the morphed normals:
            mynormals = moglsingle([ normals ; zeros(1, size(normals, 2))]);
            Screen('BeginOpenGL', win);
            glBindBuffer(GL.ARRAY_BUFFER, vbo);
            glBufferSubData(GL.ARRAY_BUFFER, vbonormalstart, (4 * size(normals, 2) * 4), mynormals);
            glBindBuffer(GL.ARRAY_BUFFER, 0);
            Screen('EndOpenGL', win);
        end
        
        if usecolors
            buffersize = size(vertcolors,1) * size(vertcolors,2) * 4;
            if objcount == 1
                colorvbo = glGenBuffers(1);
                glBindBuffer(GL.ARRAY_BUFFER, colorvbo);
                glBufferData(GL.ARRAY_BUFFER, buffersize, 0, GL.STATIC_DRAW);
            else
                glBindBuffer(GL.ARRAY_BUFFER, colorvbo);                
                glBufferSubData(GL.ARRAY_BUFFER, 0, buffersize, moglsingle(vertcolors));
            end
            glBindBuffer(GL.ARRAY_BUFFER, 0);
        end
        
        clear invertices;
        
        if IsOpenGLRendering
            % Reenable OpenGL mode:
            Screen('BeginOpenGL', targetwindow);
        end
    end
        
    % Increment total count of updates:
    updatecount = updatecount + 1;

    % Return a handle to the new keymesh:
    rc = objcount;
    return;
end;

if strcmpi(cmd, 'getGeometry')
    % Return current internal vertex and normals array:

    if updatecount <= 1
        error('Tried to retrieve morphed geometry/normals, but "computeMorph" not yet called at least once!');
    end;

    % Compute intense resync of GPU's VBO with our cached copies needed?
    if resynccount < updatecount
        % Yes: Do it.
        % In cpu only mode, we have our results already in internal persistent
        % matrices "vertices" and "normals", so nothing complex to do.
        % In gpu mode, we need to fetch data from the GPU aka the vbo we use:
        if gpubasedmorphing
            % Query current OpenGL state:
            [targetwindow, IsOpenGLRendering] = Screen('GetOpenGLDrawMode');

            if IsOpenGLRendering
                % Disable OpenGL mode:
                Screen('EndOpenGL', targetwindow);
            end

            % Get [w,h] width x height of 2D morphbuffer in "data pixels". Each
            % datapixel is either a 4D-vertex coordinate or a 4D-normal vector,
            % the 4th component being padding:
            [w, h] = Screen('WindowSize', morphbuffer(1));

            % Total buffer size is w*h elements, each with 4 components (4D)
            % and each component being a 4-Byte float value. As half of this
            % amount is vertex data and the other half is normal data, we
            % divide by 2 to get amount of vertex / normal data to retrieve:
            buffersize = w * h * 4 * 4 / 2;

            Screen('BeginOpenGL', morphbuffer(1));

            % Bind vbo with current (morphed) geometry (positions and normals):
            glBindBuffer(GL.ARRAY_BUFFER, vbo);

            vertices = zeros(4, buffersize / 4 / 4);
            vertices = moglsingle(vertices);
            glGetBufferSubData(GL.ARRAY_BUFFER, vbovertexstart, buffersize, vertices);
            vertices = vertices(1:3, 1:realVertexCount);

            if nargout > 1 && usenormals
                normals = zeros(4, buffersize / 4 / 4);
                normals = moglsingle(normals);
                glGetBufferSubData(GL.ARRAY_BUFFER, vbonormalstart, buffersize, normals);
                normals = normals(1:3, 1:realNormalCount);
            end

            % Done. Unbind vbo:
            glBindBuffer(GL.ARRAY_BUFFER, 0);

            Screen('EndOpenGL', morphbuffer(1));

            if IsOpenGLRendering
                % Reenable OpenGL mode:
                Screen('BeginOpenGL', targetwindow);
            end
        end
        
        % Resynced:
        resynccount = updatecount;
    end
    
    % Return our internal arrays with current morph/render results:
    rc = vertices;
    
    if nargout > 1 && usenormals
        varargout{1} = normals;
    end
    
    return;
end

if strcmpi(cmd, 'renderMesh')
    % A single mesh should be rendered.
    if nargin < 2
        error('You need to supply the handle of the mesh to be rendered!');
    end;
    
    if arg1 <1 || arg1>objcount
        error('Handle for non-existent keyshape provided!');
    end;
    
    if ~gpubasedmorphing
        % CPU based operation: 
      
        % Copy vertices and normals to renderbuffers:
        vertices(:,:) = keyvertices(:, :, arg1);
        if usenormals
            normals(:,:) = keynormals(:,:,arg1);
        end;
    else
        % GPU morphing ops: Our geometry is stored in textures...
        
        % Query current OpenGL state:
        [targetwindow, IsOpenGLRendering] = Screen('GetOpenGLDrawMode');

        if IsOpenGLRendering
            % Disable OpenGL mode:
            Screen('EndOpenGL', targetwindow);
        end

        [w,h] = Screen('WindowSize', keyshapes(arg1));

        Screen('BeginOpenGL', keyshapes(arg1));
        glBindBuffer(GL.PIXEL_PACK_BUFFER_ARB, vbo);
        
        % Read back whole shape texture, including normals:
        glReadPixels(0, 0, w, h, GL.RGBA, GL.FLOAT, vbovertexstart);

        glBindBuffer(GL.PIXEL_PACK_BUFFER_ARB, 0);
        Screen('EndOpenGL', keyshapes(arg1));

        if IsOpenGLRendering
            % Reenable OpenGL mode:
            Screen('BeginOpenGL', targetwindow);
        end
    end
    
    % Increment global update counter:
    updatecount = updatecount + 1;

    % Set command code for rendering current content of renderbuffers:
    cmd = 'render';
end;

if strcmpi(cmd, 'renderMorph') || strcmpi(cmd, 'computeMorph')
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
    
    if size(arg1, 2)~=1
        arg1 = transpose(arg1);
    end

    % Make sure the weights are in double() format:
    arg1 = double(arg1);
    
    % By default we morph normal vectors as well. As this is not mathematically correct,
    % the parent-code can prevent normal morphing by providing the optional morphnormals=0
    % flag.
    if nargin < 3
        morphnormals=1;
    else
        morphnormals=arg2;
    end;

    if ~gpubasedmorphing
        % CPU based morphing via Matlabs vector operations:

        % Perform morphing of shape and normal vectors:
        vertices(:,:) = keyvertices(:,:,1) * arg1(1);
        for i=2:length(arg1)
            if abs(arg1(i)) > 0
                % Add next keyshape:
                vertices(:,:) = vertices(:,:) + (keyvertices(:,:,i) * arg1(i));
            end
        end;

        % Perform morphing of normals as well. This is not strictly correct.
        if usenormals && morphnormals
            normals(:,:) = keynormals(:,:,1) * arg1(1);
            for i=2:length(arg1)
                if abs(arg1(i)) > 0
                    % Add next keyshape:
                    normals(:,:) = normals(:,:) + (keynormals(:,:,i) * arg1(i));
                end
            end;
        end;
    else
        % GPU based morphing:
    
        % Query current OpenGL state:
        [targetwindow, IsOpenGLRendering] = Screen('GetOpenGLDrawMode');

        if IsOpenGLRendering
            % Disable OpenGL mode:
            Screen('EndOpenGL', targetwindow);
        end

        % Targetwindow active?
        if targetwindow == 0
            % Nope! We are without targetwindow, e.g., due to return from
            % an async flip operation. That means that no OpenGL context is
            % active for this thread and all OpenGL ops would be no-ops or
            % crashes. We perform a dummy call for the sole purpose to get
            % our window bound and an OpenGL context activated:
            dummyWinfo = Screen('GetWindowInfo', win); %#ok<NASGU>
        end
        
        % Switch to OpenGL mode and copy morphbuffer into our VBO:
        [w, h] = Screen('WindowSize', morphbuffer(1));

        % Do we already have a master keyshape texture which collects all
        % keyshapes (textures) into one single big texture?
        if isempty(masterkeyshapetex)
            % No. This must be the first invocation after a call to
            % 'addMesh'. Try to build the huge keyshape texture:
            if (h * objcount < glGetIntegerv(GL.MAX_RECTANGLE_TEXTURE_SIZE_EXT))
                % Number and size of keyshapes fits within contraints of
                % hardware. Build unified keyshape texture:
                masterkeyshapetex = Screen('OpenOffscreenWindow', win, [0 0 0 0], [0 0 w h*objcount], 128);
                for i=1:objcount
                    Screen('DrawTexture', masterkeyshapetex, keyshapes(objcount+1-i), [], OffsetRect([0 0 w h], 0, h*(i-1)), 0, 0);
                end
            
                % Store total count of objects to morph in shader:
                if isbuggyatidriver
                    driverslack = 1;
                else
                    driverslack = 0;
                end
                
                glUseProgram(multimorphshader);
                glUniform1i(multishadermorphcount, objcount + driverslack);
                glUseProgram(0);
            end
        end

        % Which path to use?
        if isempty(masterkeyshapetex) || (useSparseMorph > 0)
            % Iterative path. Used when number and size of keyshapes
            % doesn't fit into hardware constraints for a single shape
            % texture.
            % We iterate over all keyshape() textures, and morph-in (aka
            % multiply-accumulate) one keyshape per iteration:

            % Initialize our morphbuffer(1) offscreen window with empty shape:
            Screen('Blendfunction', morphbuffer(1), GL_ONE, GL_ZERO);
            Screen('Blendfunction', morphbuffer(2), GL_ONE, GL_ZERO);
            Screen('FillRect', morphbuffer(1), [0 0 0 0]);

            % Initial src- dst- assignement for buffer-pingpong:
            currentsrcbuffer = 1;
            currentdstbuffer = 2;

            % Morph-Loop for multiply-accumulate morph operation:
            for i=1:length(arg1)
                % Only process keyshapes with non-zero activation weight:
                if abs(arg1(i)) > 0
                    % Store morph weight for i'th shape in uniform for shader:
                    glUseProgram(morphshader);
                    glUniform1f(shadermorphweight, arg1(i));
                    glUseProgram(0);

                    % xform pass: Blit sum of new keyshape texture and currentsrcbuffer
                    % morphbuffer into currentdstbuffer morphbuffer, applying proper
                    % morph weight:
                    if morphnormals
                        % Normal morphing requested: Run full operator...
                        morphbuffer(currentdstbuffer) = Screen('TransformTexture', keyshapes(i), morphOperator, morphbuffer(currentsrcbuffer), morphbuffer(currentdstbuffer));
                    else
                        % No normal morphing: Run restricted operator...
                        morphbuffer(currentdstbuffer) = Screen('TransformTexture', keyshapes(i), morphOperatorNoNormals, morphbuffer(currentsrcbuffer), morphbuffer(currentdstbuffer));
                    end

                    % Switch source and destination buffers for next morph-pass:
                    j = currentsrcbuffer;
                    currentsrcbuffer = currentdstbuffer;
                    currentdstbuffer = j;
                end
                % Next morph-iteration...
            end
        else
            % Single pass case: All shapes fit into one texture. Assign all
            % morph-weights to a weight texture.
            % Perform whole morph in one single blit-operation:
            currentsrcbuffer = 1;

            if length(arg1) ~= oldWeightLength
                oldWeightLength = length(arg1);
                
                if ~isempty(weighttex)
                    % Release weight texture:
                    Screen('Close', weighttex);
                    weighttex = [];
                end
            end
            
            % Convert weight-vector into float texture:
            if isempty(weighttex)
                % Doesn't exist yet: Create it:
                weighttex = Screen('MakeTexture', win, arg1, [], [], 2, 2);
                weighttexgl = Screen('GetOpenGLTexture', win, weighttex);
            else
                % weighttex exists already and can be recycled. Just bind
                % it and set new weights in it:
                glBindTexture(GL.TEXTURE_RECTANGLE_EXT, weighttexgl);
                glTexSubImage2D(GL.TEXTURE_RECTANGLE_EXT, 0, 0, 0, oldWeightLength, 1, GL.LUMINANCE, GL.FLOAT, moglsingle(arg1));
                glBindTexture(GL.TEXTURE_RECTANGLE_EXT, 0);
            end
            
            % xform pass: Blit all subsections of masterkeyshapetex into
            % the destination buffer, applying the multimorph-shader:
            if morphnormals
                % Normal morphing requested: Run full operator...
                morphbuffer(currentsrcbuffer) = Screen('TransformTexture', masterkeyshapetex, multimorphOperator, weighttex, morphbuffer(currentsrcbuffer));
            else
                % No normal morphing: Run restricted operator...
                morphbuffer(currentsrcbuffer) = Screen('TransformTexture', masterkeyshapetex, multimorphOperatorNoNormals, weighttex, morphbuffer(currentsrcbuffer));
            end
        end

        % Ok, morphbuffer(currentsrcbuffer) should contain the final morph
        % result:

        % DEBUG CODE: Readout morph result to Matlab:
        %    intex1 = Screen('GetImage', morphbuffer(currentsrcbuffer), [], [], 1);
        %    imshow(intex1);
        %    minimum=    min(min(min(intex1)))
        %    maximum=    max(max(max(intex1)))

        if IsOSX
            % I have no clue why this glFlush helps to prevent a crash on OS/X
            % 10.4.10 with ATI Radeon X1600 when usercode has alpha-blending
            % enabled, but it does. Without this glFlush() everything fine on
            % WindowsXP+NVidia, but crash on OS/X ATI...
            % Well, some clue, after some exchange on the Apple mailing lists,
            % it seems to be an ATI driver bug...
            glFlush;
        end

        % Switch to OpenGL mode and copy morphbuffer into our VBO:
        % We use PTB's internal context here, our state mods should be safe
        % for Screen():
        Screen('BeginOpenGL', morphbuffer(currentsrcbuffer), 1);

        glBindBuffer(GL.PIXEL_PACK_BUFFER_ARB, vbo);

        % Disable alpha-blending around glReadPixels if its enabled. While
        % this is not needed on OS/X, NVidia Geforce 7800 under WinXP falls
        % back to software path if alpha-blending is on... Doesn't make
        % sense to me, maybe a driver bug?
        alphablending = glIsEnabled(GL.BLEND);
        if alphablending
            glDisable(GL.BLEND);
        end
        
        % Do we have normals, and if so, do we want to morph them?
        if ~morphnormals && usenormals
            % We have normals, but don't wanna morph them: Only read back
            % half of the morphbuffers height, so we omit copying the new
            % morphed normals to the normal-array section of our VBO:
            glReadPixels(0, 0, w, h/2, GL.RGBA, GL.FLOAT, vbovertexstart);
        else
            % We either don't have normals or we have 'em and want to morph
            % them. In both cases, readback the whole morphbuffer:
            glReadPixels(0, 0, w, h, GL.RGBA, GL.FLOAT, vbovertexstart);
        end

        if alphablending
            % Reenable alpha blending if it got disabled by us:
            glEnable(GL.BLEND);
        end

        glBindBuffer(GL.PIXEL_PACK_BUFFER_ARB, 0);
        Screen('EndOpenGL', morphbuffer(currentsrcbuffer));
        
        if IsOpenGLRendering
            % Reenable OpenGL mode:
            Screen('BeginOpenGL', targetwindow);
        end

        % End of GPU based morphing:
    end

    % Increment total update counter:
    updatecount = updatecount + 1;

    % Just morph or render as well?
    if strcmpi(cmd, 'renderMorph')
        % Set command code for rendering current content of renderbuffers:
        cmd = 'render';
    else
        % Only morphing requested, not rendering. Morph can be rendered via
        % 'rerender' command later.
        return;
    end;
end;

if strcmpi(cmd, 'renderNormals')
    
    if updatecount < 1
        error('Tried to render normals in renderbuffer, but renderbuffer not yet filled!');
    end;
    
    if gpubasedmorphing
        % NOT YET IMPLEMENTED: Skip with warning:
        % warning('moglmorpher: Subcommand "renderNormals" not yet implemented for GPU based morphing!');
        % return;

        if resynccount < updatecount
            % Recursive call to ourselves to update our internal vertices and
            % normals matrices with readback data from the GPU's VBO:
            [vertices, normals] = moglmorpher('getGeometry');
        end
    end

    if (size(vertices,2)~=size(normals,2))
        error('renderNormals: Sorry this function only works if count of normals equals count of vertices. Aborted!');
    end;

    if nargin < 2
        arg1 = 1;
    end;
    
    % Loop over all vertices and draw their corresponding normals:
    % This is OpenGL immediate-mode rendering, so it will be slow...
    glBegin(GL.LINES);
    for i=1:size(vertices,2)
        % Start position of normal is vertex position:
        glVertex3dv(vertices(:,i));
        % End position is defined by scaled normal vector: Argument 1 defines length of normal:
        glVertex3dv(vertices(:,i) + (normals(:,i)/norm(normals(:,i)))*arg1);
    end;
    glEnd;
    
    % Done.
    return;
end;

if strcmpi(cmd, 'render') || strcmpi(cmd, 'renderToDisplaylist')
    % Render current content of renderbuffers via OpenGL:
    if updatecount < 1
        error('Tried to render content of renderbuffers, but renderbuffers not yet filled!');
    end;

    if strcmpi(cmd, 'renderToDisplaylist')
        % Create new display list and direct all rendering into it:
        rc = glGenLists(1);
        glNewList(rc, GL.COMPILE);
    else
        % Just render, don't create display list.
        rc = -1;
    end;

    % Enable client-side vertex arrays:
    glEnableClientState(GL.VERTEX_ARRAY);
    
    if ~gpubasedmorphing
        % CPU based rendering: Submit all data from host memory to GPU
        % memory and render:
        
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

        if usecolors
            % Enable client-side texture coordinate arrays:
            glEnableClientState(GL.COLOR_ARRAY);
            % Set pointer to start of vertex color array:
            glColorPointer(size(vertcolors, 1), usetype, 0, vertcolors);
        end;

        % Render mesh, using topology given by 'faces':
        if drawrangeelements
            % Faster rendering-path, needs OpenGL-1.2 or higher:
            if (size(faces,1)==3)
                glDrawRangeElements(GL.TRIANGLES, minvertex, maxvertex, size(faces,2) * 3, GL.UNSIGNED_INT, faces);
            elseif size(faces,1)==4
                glDrawRangeElements(GL.QUADS, minvertex, maxvertex, size(faces,2) * 4, GL.UNSIGNED_INT, faces);
            else
                error('Invalid number of rows in face index array!');
            end;
        else
            % Slower rendering path, needed to support OpenGL-1.1 renderers as well:
            if (size(faces,1)==3)
                glDrawElements(GL.TRIANGLES, size(faces,2) * 3, GL.UNSIGNED_INT, faces);
            elseif size(faces,1)==4
                glDrawElements(GL.QUADS, size(faces,2) * 4, GL.UNSIGNED_INT, faces);
            else
                error('Invalid number of rows in face index array!');
            end;
        end;
    else
        % GPU based rendering: Setup buffer mappings for our VBO's, then
        % render:
        glBindBuffer(GL.ARRAY_BUFFER, vbo);
        glVertexPointer(4, GL.FLOAT, 0, vbovertexstart);

        if usetextures
            % Enable texture coordinate arrays:
            glEnableClientState(GL.TEXTURE_COORD_ARRAY);
            % Set pointer to start of texcoord array:
            glTexCoordPointer(size(texcoords, 1), GL.FLOAT, 0, 0);
        else
            glDisableClientState(GL.TEXTURE_COORD_ARRAY);
        end;

        if usenormals
            % Enable normal vector arrays:
            glEnableClientState(GL.NORMAL_ARRAY);
            % Set pointer to start of normal array:
            glNormalPointer(GL.FLOAT, 4*4, vbonormalstart);
        else
            glDisableClientState(GL.NORMAL_ARRAY);
        end

        if usecolors
            % Enable client-side texture coordinate arrays:
            glEnableClientState(GL.COLOR_ARRAY);

            % We use a separate dedicated VBO for vertex colors:
            glBindBuffer(GL.ARRAY_BUFFER, colorvbo);

            % Set pointer to start of vertex color buffer:
            glColorPointer(size(vertcolors, 1), usetype, 0, 0);
        else
            glDisableClientState(GL.COLOR_ARRAY);
        end
        
        % Bind face index VBO 'ibo':
        glBindBuffer(GL.ELEMENT_ARRAY_BUFFER_ARB, ibo);

        % Render mesh, using topology given by 'faces':
        if (size(faces,1)==3)
            glDrawRangeElements(GL.TRIANGLES, minvertex, maxvertex, size(faces,2) * 3, GL.UNSIGNED_INT, 0);
        elseif size(faces,1)==4
            glDrawRangeElements(GL.QUADS, minvertex, maxvertex, size(faces,2) * 4, GL.UNSIGNED_INT, 0);
        else
            error('Invalid number of rows in face index array!');
        end;

        % Unbind our VBOs:
        glBindBuffer(GL.ELEMENT_ARRAY_BUFFER_ARB, 0);
        glBindBuffer(GL.ARRAY_BUFFER, 0);

        % End of GPU VBO rendering:
    end
    
    % Common code path for CPU and GPU based rendering:
    
    % Disable vertex, normals and texcoord arrays:
    glDisableClientState(GL.VERTEX_ARRAY);
    glDisableClientState(GL.NORMAL_ARRAY);
    glDisableClientState(GL.TEXTURE_COORD_ARRAY);
    glDisableClientState(GL.COLOR_ARRAY);

    if rc>-1
        % Finalize our new display list:
        glEndList;
    end;
    
    return;
end;

if strcmpi(cmd, 'getVertexPositions')   
   % Calling routine wants projected screen space vertex positions of all vertices
   % in our current renderbuffer.
   
   if nargin < 2 || isempty(arg1)
      error('win Windowhandle missing in call to getVertexPositions!')
   end;
   
   if nargin < 3
      startidx = 1;
   else
      if isempty(arg2)
         startidx = 1;
      else
         startidx = arg2;
      end;
   end;
   
   if nargin < 4
      endidx = size(vertices,2);
   else
      if isempty(arg3)
         endidx = size(vertices,2);
      else
         endidx = arg3;
      end;
   end;
   
   % Correct for 0-start of OpenGL/C vs. 1-start of Matlab:
   startidx = startidx - 1;
   endidx = endidx - 1;
   
   % Total count of vertices to handle:
   ntotal = endidx - startidx + 1;
   
   % We put OpenGL into feedback mode, do a pure point rendering pass, switch back to
   % normal mode and return the content of the feedback buffer in an easy format.
   
   % Compute needed capacity of feedbackbuffer, assuming all vertices in the buffer
   % get transformed and none gets clipped away:
   reqbuffersize = ntotal * 4 * 4; % numVertices * 4 float/vertex * 4 bytes/float.
   
   % Feedback buffer already allocated in proper size?
   if feedbackptr~=0 && feedbacksize < reqbuffersize
      % Allocated, but too small for our purpose. Delete & Reallocate:
      moglfree(feedbackptr);
      feedbackptr=0;
      feedbacksize=0;
   end;
   
   % Feedback buffer ready?
   if feedbackptr==0
      % Nope. Need to allocate a new one:
      feedbackptr=moglmalloc(reqbuffersize);
      feedbacksize=reqbuffersize;
   end;
   
   % Our feedback memory buffer is ready. Assign it to the GL: We request the
   % full transformed 3D pos of the vertex:
   glFeedbackBuffer(reqbuffersize/4, GL.GL_3D, feedbackptr);
   
   % Enable client-side vertex arrays:
   glEnableClientState(GL.VERTEX_ARRAY);
   
   if gpubasedmorphing
       % Set pointer to start of vertex array and bind our VBO:
       glBindBuffer(GL.ARRAY_BUFFER, vbo);
       glVertexPointer(4, GL.FLOAT, 0, vbovertexstart);
   else
       % Set pointer to start of vertex array:
       glVertexPointer(size(vertices,1), usetype, 0, vertices);
   end
   
   % Put OpenGL into feedback mode:
   glRenderMode(GL.FEEDBACK);
   
   % Render all vertices: This does not draw, but just transform the vertices
   % into projected screen space and returns their 3D positions in the feedback-buffer:
   glDrawArrays(GL.POINTS, startidx, ntotal);
   
   if gpubasedmorphing
       % Unbind our VBO:
       glBindBuffer(GL.ARRAY_BUFFER, 0);
   end

   % Disable client-side vertex arrays:
   glDisableClientState(GL.VERTEX_ARRAY);
   
   % Put OpenGL back into normal mode and get number of items:
   nritems = glRenderMode(GL.RENDER);
   
   % Copy content of buffer into a linear matrix:
   if usetype == GL.FLOAT
      tmpbuffer = moglgetbuffer(feedbackptr, GL.FLOAT, nritems * 4);
   else
      % No single precision float's available :( - Need to do our
      % ugly trick: Request data as uint32 array, then use our special cast routine to upcast it to a double matrix.
      tmpbuffer = moglgetbuffer(feedbackptr, GL.UNSIGNED_INT, nritems * 4);
      tmpbuffer = mogldouble(tmpbuffer);
   end;

   % Reshape it to be a n-by-4 matrix:
   tmpbuffer = transpose(reshape(tmpbuffer, 4, floor(nritems / 4)));
   % Cast to double, throw away token column:
   rc(:,1:3) = double(tmpbuffer(:,2:4));
   % Invert y-coordinates so they match again:
   rc(:,2)   = RectHeight(Screen('Rect', arg1)) - rc(:,2);
   
   % Done. Return array in rc:
   return;
end;

% Take vector of textures and matching vector of morphWeights and linearly
% combine those textures according to the weights. Return handles to
% resulting texture:
if strcmpi(cmd, 'morphTexture')
    if nargin < 3 || isempty(arg1) || isempty(arg2) || isempty(arg3)
        error('morphTexture: At least one of the required arguments windowPtr, weights or keytextures is missing!');
    end

    if ~gpubasedmorphing
        highprec = 0;
    else
        highprec = 1;
    end

    win = arg1;
    weights = arg2;
    texkeyshapes = arg3;

    refrect = Screen('Rect', texkeyshapes(1));
    for i=2:length(texkeyshapes)
        if ~isequal(refrect, Screen('Rect', texkeyshapes(i)))
            error('morphTexture: Not all passed keytextures have the same size (width x height) as required!');
        end
    end

    if length(weights) > length(texkeyshapes)
        error('morphTexture: Vector of weights has more components than vector of keytextures!');
    end

    % Buffers exist, but don't match the size of the keytextures?
    if ~isempty(texmorphbuffer) && ~isequal(refrect, Screen('Rect', texmorphbuffer(1)))
        % Release current buffers:
        Screen('Close', texmorphbuffer);
        texmorphbuffer = [];
    end

    % (Re-)Create morph buffers if they don't already exist:
    if isempty(texmorphbuffer)
        if highprec
            % Create float buffers of matching size:
            texmorphbuffer(1) = Screen('OpenOffscreenWindow', win, [0 0 0 0], refrect, 128);
            texmorphbuffer(2) = Screen('OpenOffscreenWindow', win, [0 0 0 0], refrect, 128);

            % Setup proper blend mode:
            Screen('Blendfunction', texmorphbuffer(1), GL_ONE, GL_ZERO);
            Screen('Blendfunction', texmorphbuffer(2), GL_ONE, GL_ZERO);
        else
            % Create low precision buffer of matching size:
            texmorphbuffer(1) = Screen('OpenOffscreenWindow', win, [0 0 0 0], refrect, 32);

            % Setup proper blend mode for morphing via blending: alpha
            % value will define blend weight:
            Screen('Blendfunction', texmorphbuffer(1), GL_SRC_ALPHA, GL_ONE);
        end
    else
        % Initialize our texmorphbuffer offscreen windows with empty textures:
        if highprec
            Screen('FillRect', texmorphbuffer(1), [0 0 0 0]);
        else
            % Prefill buffer with 1st morphed texture:
            Screen('Blendfunction', texmorphbuffer(1), GL_SRC_ALPHA, GL_ZERO);
            Screen('DrawTexture', texmorphbuffer(1), texkeyshapes(1), [], [], [], 0, weights(1));
            Screen('Blendfunction', texmorphbuffer(1), GL_SRC_ALPHA, GL_ONE);
        end
    end

    if gpubasedmorphing && isempty(texmorphOperator)
        texmorphshader = LoadGLSLProgramFromFiles('LinearCombinationOfTwoImagesShader.frag.txt');
        texmorphOperator = CreateGLOperator(win, kPsychNeed32BPCFloat, texmorphshader, 'Multiply-Accumulate operator for texture morphing');

        % Need 'shadermorphweight' uniform location, so we can assign a
        % different weight for morphing of each keyshape in morph-loop:
        texshadermorphweight = glGetUniformLocation(texmorphshader, 'Image1Weight');

        % Setup all other uniforms to their fixed values:
        glUseProgram(texmorphshader);

        % Don't need 2nd weight of this shader for our purpose: Set it
        % to 1.0 so we get a nice multiply-accumulate behaviour:
        glUniform1f(glGetUniformLocation(texmorphshader, 'Image2Weight'), 1.0);

        % Assign texture units to samplers:
        glUniform1i(glGetUniformLocation(texmorphshader, 'Image1'), 0);
        glUniform1i(glGetUniformLocation(texmorphshader, 'Image2'), 1);
        glUseProgram(0);
    end

    if highprec
        % We iterate over all keytextures, and morph-in (aka
        % multiply-accumulate) one keytexture per iteration:

        % Initial src- dst- assignement for buffer-pingpong:
        currentsrcbuffer = 1;
        currentdstbuffer = 2;

        % Morph-Loop for multiply-accumulate morph operation:
        for i=1:length(weights)
            % Only process keytextures with non-zero activation weight:
            if abs(weights(i)) > 0
                % Store morph weight for i'th texture in uniform for shader:
                glUseProgram(texmorphshader);
                glUniform1f(texshadermorphweight, weights(i));
                glUseProgram(0);

                % xform pass: Blit sum of new keytexture and currentsrcbuffer
                % into currentdstbuffer, applying proper morph weight:
                texmorphbuffer(currentdstbuffer) = Screen('TransformTexture', texkeyshapes(i), texmorphOperator, texmorphbuffer(currentsrcbuffer), texmorphbuffer(currentdstbuffer));

                % Switch source and destination buffers for next morph-pass:
                j = currentsrcbuffer;
                currentsrcbuffer = currentdstbuffer;
                currentdstbuffer = j;
            end
            % Next morph-iteration...
        end
    else
        % Fast path: Less accurate, but works on old gpu's.
        currentsrcbuffer = 1;

        if any(weights < 0)
            fprintf('moglmorpher: morphTexture: WARNING! Running in shaderless low-precision mode and some\n');
            fprintf('moglmorpher: morphTexture: morph weights are < 0. I can''t handle this correctly, results\n');
            fprintf('moglmorpher: morphTexture: will be wrong! Enable gpu based morphing for me to cope with this.\n');
        end

        for i=2:length(weights)
            % Only process keytextures with non-zero activation weight:
            if weights(i) > 0
                % Draw i'th texkeyshape (weighted by weight(i)) on top of
                % texmorphbuffer(1), thereby multiply-adding it due to the
                % selected blending mode:
                Screen('DrawTexture', texmorphbuffer(1), texkeyshapes(i), [], [], [], 0, weights(i));
            end
            % Next morph-iteration...
        end
    end

    % outtex is handle to final texture:
    outtex = texmorphbuffer(currentsrcbuffer);

    % Return ptb handle as 1st argument:
    rc = outtex;

    % Return opengl handle and gltexture target as 2nd and 3rd argument:
    [varargout{1}, varargout{2}] = Screen('GetOpenGLTexture', win, outtex);

    return;
end;

error('Invalid subcommand specified!');
return; %#ok<UNRCH>
