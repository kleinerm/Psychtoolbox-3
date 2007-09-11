function rc = moglmorpher(cmd, arg1, arg2, arg3, arg4)
%
% Matlab OpenGL Morpher - Performs linear morphs between different 3D shapes and
% renders the resulting shape via OpenGL. Supports high-performance GPU
% based morphing on recent graphics hardware.
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
% For a specific example of usage, have a look at MorphDemo.m
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
% moglmorpher('renderNormals' [,normalLength=1]);
% -- Renders the surface normal vectors of the last shape in green, either at unit-length,
% or at 'normalLength' if this argument is provided. This is a helper function for
% checking the correctness of computed normals. It is very slow!
%
% vpos = moglmorpher('getVertexPositions', windowPtr [, startidx=1] [, endidx]);
% -- Compute and return a matrix which contains the projected screen space coordinates of all
% vertices that would be rendered when calling moglmorpher('render'). windowPtr is the handle
% of the window into which we render. Optional arguments startidx and endidx define the
% index of the first vertex, resp. the last vertex to transform. The returned 'vpos' is a
% vcount-by-3 matrix, where vcount is the number of returned vertices, and row i contains the
% projected 3D position of the i'th vertex vcount(i,:) = (screen_x, screen_y, depth_z);
%
% count = moglmorpher('getMeshCount');
% Returns number of stored shapes.
%
% moglmorpher('reset');
% Resets the moglmorpher - deletes all internal data structures.

%
% rc = moglmorpher(cmd, arg1, arg2, arg3, arg4)

% (c) 2006 Mario Kleiner - licensed to you under GPL.
%
% History:
% Sometimes 2006 - Written (MK).
% 10.09.2007  Added code for high-performance GPU based morhping. (MK).
%             This makes use of FBOs, PBOs and VBOs plus the PTB imaging
%             pipeline to perform all morphing on the GPU, ie., all data
%             always resides in VRAM, only minimal involvement of the CPU
%             and host<->GPU bus --> Should provide significant speedups
%             for large meshes or many meshes. Observed speedup on
%             MacBookPro with ATI Radeon X1600 over Matlab7.4 is 3x.

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

% glDrawRangeElements() command supported?
persistent drawrangeelements;

% Pointer to and size of current memory feedback buffer:
persistent feedbackptr;
persistent feedbacksize;

% Internal state needed for fully GPU based implementation:
persistent win;
persistent gpubasedmorphing;
persistent keyshapes;
persistent morphbuffer;
persistent vbo;
persistent ibo;

persistent vbovertexstart;
persistent vbonormalstart;

persistent morphshader;
persistent morphOperator;
persistent shadermorphweight;

% Sanity check:
if nargin < 1
    help moglmorpher;
    return;
end;

% Special subcommand: The only one to be called without open onscreen
% window:
if strcmp(cmd, 'ForceGPUMorphingEnabled')
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

if isempty(gpubasedmorphing)
    % Test if fully GPU based morphing via FBO's VBO's PBO's and shaders is
    % possible. Use it, if so:
    gpubasedmorphing = 0;
    
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

        if (bitand(winfo.ImagingMode, kPsychNeedFastBackingStore)==0) & (winfo.ImagingMode~= kPsychNeedFastOffscreenWindows)
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
    updatecount = 0;

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
if strcmp(cmd, 'reset')
   % Reset ourselves:
   
   if (gpubasedmorphing > 0) & (objcount > 0)
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
       
       % Release Offscreen windows:
       Screen('Close', morphbuffer(1));
       Screen('Close', morphbuffer(2));
       morphbuffer = [];
       
       % Release shader:
       shadermorphweights = [];
       Screen('Close', morphOperator);
       morphOperator = [];
       glDeleteProgram(morphshader);
       morphshader = [];

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
   faces = [];
   keyvertices = [];
   keynormals = [];
   win = [];
   rc = 0;
   
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
        vertices = moglsingle(arg2);
    else
        vertices = double(arg2);
    end;

    % 3rd (optional) argument is texture coordinate array:
    if argcount>3 && ~isempty(arg3)
        if usetype==GL.FLOAT
            texcoords = moglsingle(arg3);
        else
            texcoords = double(arg3);
        end;
        usetextures = 1;    
    end;
    
    % 4th (optional) argument is normal vectors array:
    if argcount>4 && ~isempty(arg4)
        if usetype==GL.FLOAT
            normals = moglsingle(arg4);
        else
            normals = double(arg4);
        end;
        usenormals = 1;
    end;

    % Assign vertex array and normals array to new slot in keyshape vector:
    objcount = objcount + 1;
    
    if ~gpubasedmorphing
        % CPU based operation: Just copy vertices and normals into our
        % internal arrays of keyshapes:
        keyvertices(:,:,objcount)=vertices(:,:);
        keynormals(:,:,objcount)=normals(:,:);
    else    
        % GPU based operation: Setup "shape textures" ie, floating point
        % textures that encode vertex and normal vectors.

        % Build shape vector texture:
        texlinelength = 1024;
        nrows = ceil(size(vertices, 2) / texlinelength);
        ncols = texlinelength;
        npad = (texlinelength - mod(size(vertices, 2), texlinelength));
        %oldsize = size(vertices)

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

        %newsize = size(invertices)

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
        %myshapesize = size(myshapeimg)
        
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
           
        % First time invocation?
        if objcount == 1
            % Yes! Need to allocate and setup FBO's PBO's and VBO's and create
            % GLSL operator for morphing on GPU:

            % Creation and init of special GLSL morph-operator for shape morphing
            % on GPU:
            morphshader = LoadGLSLProgramFromFiles('LinearCombinationOfTwoImagesShader.frag.txt');
            morphOperator = CreateGLOperator(win, kPsychNeed32BPCFloat, morphshader, 'Multiply-Accumulate operator for shape morphing');

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

            buffersize = arraycount * ncols*nrows*3*4;
            
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
                glBufferSubData(GL.ARRAY_BUFFER, 0, texcoordsize, texcoords);
                vbovertexstart = texcoordsize;
            end
            
            if usenormals
                vbonormalstart = vbovertexstart + (size(invertices, 1) * size(invertices, 2) * 4);
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
            Screen('BeginOpenGL', win);
            glBindBuffer(GL.ARRAY_BUFFER, vbo);
            glBufferSubData(GL.ARRAY_BUFFER, vbonormalstart, (size(normals, 1) * size(normals, 2) * 4), normals);
            glBindBuffer(GL.ARRAY_BUFFER, 0);
            Screen('EndOpenGL', win);
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

if strcmp(cmd, 'renderMesh')
    % A single mesh should be rendered.
    if nargin < 2
        error('You need to supply the handle of the mesh to be rendered!');
    end;
    
    if arg1 <1 | arg1>objcount
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
        glReadPixels(0, 0, w, h, GL.RGB, GL.FLOAT, vbovertexstart);

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

    if ~gpubasedmorphing
        % CPU based morphing via Matlabs vector operations:

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
    else
        % GPU based morphing:
    
        % Query current OpenGL state:
        [targetwindow, IsOpenGLRendering] = Screen('GetOpenGLDrawMode');

        if IsOpenGLRendering
            % Disable OpenGL mode:
            Screen('EndOpenGL', targetwindow);
        end
        
        % Initialize our morphbuffer(1) offscreen window with empty shape:
        Screen('FillRect', morphbuffer(1), [0 0 0 0]);

        % Initial src- dst- assignement for buffer-pingpong:
        currentsrcbuffer = 1;
        currentdstbuffer = 2;

        % Morph-Loop for multiply-accumulate morph operation:
        for i=1:length(arg1)
            % Store morph weight for i'th shape in uniform for shader:
            glUseProgram(morphshader);
            glUniform1f(shadermorphweight, arg1(i));
            glUseProgram(0);

            % xform pass: Blit sum of new keyshape texture and currentsrcbuffer
            % morphbuffer into currentdstbuffer morphbuffer, applying proper
            % morph weight:
            morphbuffer(currentdstbuffer) = Screen('TransformTexture', keyshapes(i), morphOperator, morphbuffer(currentsrcbuffer), morphbuffer(currentdstbuffer));

            % Switch source and destination buffers for next morph-pass:
            j = currentsrcbuffer;
            currentsrcbuffer = currentdstbuffer;
            currentdstbuffer = j;
            % Next morph-iteration...
        end

        % Ok, morphbuffer(currentsrcbuffer) should contain the final morph
        % result:

        % DEBUG CODE: Readout morph result to Matlab:
        %    intex1 = Screen('GetImage', morphbuffer(currentsrcbuffer), [], [], 1);
        %    imshow(intex1);
        %    minimum=    min(min(min(intex1)))
        %    maximum=    max(max(max(intex1)))

        % Switch to OpenGL mode and copy morphbuffer into our VBO:
        [w,h] = Screen('WindowSize', morphbuffer(currentsrcbuffer));

        Screen('BeginOpenGL', morphbuffer(currentsrcbuffer));
        glBindBuffer(GL.PIXEL_PACK_BUFFER_ARB, vbo);
        
        % Do we have normals, and if so, do we want to morph them?
        if ~morphnormals & usenormals
            % We have normals, but don't wanna morph them: Only read back
            % half of the morphbuffers height, so we omit copying the new
            % morphed normals to the normal-array section of our VBO:
            glReadPixels(0, 0, w, h/2, GL.RGB, GL.FLOAT, vbovertexstart);
        else
            % We either don't have normals or we have 'em and want to morph
            % them. In both cases, readback the whole morphbuffer:
            glReadPixels(0, 0, w, h, GL.RGB, GL.FLOAT, vbovertexstart);
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
    if strcmp(cmd, 'renderMorph')
        % Set command code for rendering current content of renderbuffers:
        cmd = 'render';
    else
        % Only morphing requested, not rendering. Morph can be rendered via
        % 'rerender' command later.
        return;
    end;
end;

if strcmp(cmd, 'renderNormals')

    if gpubasedmorphing
        % NOT YET IMPLEMENTED: Skip with warning:
        warning('moglmorpher: Subcommand "renderNormals" not yet implemented for GPU based morphing!');
        return;
    end
    
    if updatecount < 1
        error('Tried to render normals in renderbuffer, but renderbuffer not yet filled!');
    end;

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
        glVertexPointer(3, GL.FLOAT, 0, vbovertexstart);

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
            glNormalPointer(GL.FLOAT, 0, vbonormalstart);
        else
            glDisableClientState(GL.NORMAL_ARRAY);
        end

        % Bind face index VBO 'ibo':
        glBindBuffer(GL.ELEMENT_ARRAY_BUFFER_ARB, ibo);

        % Render mesh, using topology given by 'faces':
        if drawrangeelements
            % Faster rendering-path, needs OpenGL-1.2 or higher:
            if (size(faces,1)==3)
                glDrawRangeElements(GL.TRIANGLES, minvertex, maxvertex, size(faces,2) * 3, GL.UNSIGNED_INT, 0);
            elseif size(faces,1)==4
                glDrawRangeElements(GL.QUADS, minvertex, maxvertex, size(faces,2) * 4, GL.UNSIGNED_INT, 0);
            else
                error('Invalid number of rows in face index array!');
            end;
        else
            % Slower rendering path, needed to support OpenGL-1.1 renderers as well:
            if (size(faces,1)==3)
                glDrawElements(GL.TRIANGLES, size(faces,2) * 3, GL.UNSIGNED_INT, 0);
            elseif size(faces,1)==4
                glDrawElements(GL.QUADS, size(faces,2) * 4, GL.UNSIGNED_INT, 0);
            else
                error('Invalid number of rows in face index array!');
            end;
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

    if rc>-1
        % Finalize our new display list:
        glEndList;
    end;
    
    return;
end;

if strcmp(cmd, 'getVertexPositions')   
   % Calling routine wants projected screen space vertex positions of all vertices
   % in our current renderbuffer.
   
   if nargin < 2 | isempty(arg1)
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
   if feedbackptr~=0 & feedbacksize < reqbuffersize
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
       glVertexPointer(3, GL.FLOAT, 0, vbovertexstart);
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
      tmpbuffer = castDouble2Float(tmpbuffer, 1);
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

error('Invalid subcommand specified!');
return;
