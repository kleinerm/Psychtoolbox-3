function outObj = GPUTypeFromToGL(cmd, inObj, glObjType, outObj)
% outObj = GPUTypeFromToGL(cmd, inObj [, glObjType][, outObj])
%
% if cmd is zero, then convert an OpenGL object of type glObjType,
% referenced by handle inObj into a GPU object and return it in outObj. If
% the optional outObj is provided as input argument, try to recycle it --
% just fill its content with OpenGL object's content. Otherwise, create an
% outObj of matching format for content.
%
% If cmd is == 1, then convert GPU object inObj to OpenGL object of type
% glObjType and return it in outObj. Try to recycle a passed in outObj, if
% possible, otherwise create a new one.
%
% glObjType == 0 (default): Provided OpenGL object is a Psychtoolbox
% texture or offscreen window handle.
%

persistent initialized;
if isempty(initialized)
    % Start/Initialize GPUmat GPU computing toolkit if not already started:
    if ~GPUstart(1)
        GPUstart;
    end

    % Ready to rock!
    initialized = 1;
end

if nargin < 2 || isempty(cmd) || isempty(inObj) || ~isscalar(cmd) || ~isnumeric(cmd)
    error('Missing minimum required arguments "cmd" and "inObj".');
end

switch cmd
    case 0,
        % Copy from OpenGL to GPU backend:
        direction = 0;
    
    case 1,
        % Copre from GPU backend to OpenGL:
        direction = 1;
        
    case 2,
        % Purge object from cache:
        % No-Op so far.
        outObj = 1;
        return;
        
    case 3,
        % Cache invalidate:
        % No-Op so far.
        outObj = 1;
        return;
    
    otherwise
        error('Invalid cmd specified.');
end

if nargin < 3 || isempty(glObjType)
    % Psychtoolbox "classic" texture handle or offscreen window handle:
    glObjType = 0;
end

% No outObj provided for recycling?
if nargin < 4
    outObj = [];
end

if direction == 0
    % OpenGL -> GPU => gpu is outObj, if any.
    gpu = outObj;
else
    % GPU -> OpenGL => gpu is inObj:
    gpu = inObj;
    if isempty(gpu)
        error('Empty GPUtype inObj variable provided, but update of OpenGL object requested. How is this supposed to work?!?');
    end
end

% GL object is a Psychtoolbox texture/offscreen window handle?
if glObjType == 0
    % Yes.
    if direction == 0
        % OpenGL -> GPUtype conversion:
        texid = inObj;
        
        if isempty(texid)
            error('No valid Psychtoolbox OpenGL input object provided!');
        end
    else
        % GPUtype -> OpenGL conversion:
        texid = outObj;
        if isempty(texid)
            % No existing texture object provided as output destination.
            % Create a 32 bpc float texture of matching format.
            for win = Screen('Windows')
                if Screen('WindowKind', win) == 1
                    break
                end
            end
            
            if isempty(win) || Screen('WindowKind', win) ~= 1
                error('No onscreen window opened. This does not work without at least one open onscreen window.');
            end
            
            texid = Screen('MakeTexture', win, zeros(size(gpu, 3), size(gpu, 2), size(gpu, 1)), [], [], 2);
        end
    end
    
    % Get OpenGL texture handle and texture target of underlying OpenGL
    % texture for given Psychtoolbox object handle:
    [gltexid, gltextarget] = Screen('GetOpenGLTexture', texid, texid);
    
    % Retrieve width and height of texture in texels:
    [width, height] = Screen('Windowsize', texid);
    
    % Retrieve depth of texture in bytes per texel:
    bpp = Screen('Pixelsize', texid) / 8;
end

% GL object is a OpenGL object handle?
if glObjType == 1
    % TODO...
    warning('Use of OpenGL object handles not yet supported!'); %#ok<*WNTAG>
end

% GL object is color buffer attachment of currently bound FBO?
if glObjType == 2
    % TODO...
    warning('Use of currently bound OpenGL FBO not yet supported!');
end

% Number of channels:
nrchannels = bpp / 4;

if ~ismember(nrchannels, [1, 2, 4])
    error('Tried to convert a 3 layer RGB texture or framebuffer. This is not supported.');
end

% Number of bytes to copy = w * h * bpp:
nrbytes = width * height * bpp;
if nrbytes < 1
    error('Tried to convert an empty texture. Forbidden!');
end

% Is an already existing 'gpu' variable provided for "refill" ?
if ~isempty(gpu)
    % Yes: Check for matching format. If it doesn't match, delete it, so it
    % can be recreated with matching format:
    if ~isa(gpu, 'GPUsingle') || (size(gpu, 1) ~= nrchannels) || (size(gpu, 2) ~= width) || (size(gpu, 3) ~= height) || (0 == getPtr(gpu))
        % No match, or not allocated. Destroy:
        clear gpu;
        % And create empty for recreation below:
        gpu = [];
        
        % This must not happen in GPU -> OpenGL mode:
        if direction == 1
            error('Incompatible GPUtype inObj variable provided for update of OpenGL object. How is this supposed to work?!?');
        end
    end
end

% Need to create a new gpu variable?
if isempty(gpu)
    % Yes: Create a new GPUsingle GPU variable:
    gpu = GPUsingle;
    % Set it to real format:
    setReal(gpu);
    
    % Set its size:
    setSize(gpu, [nrchannels, width, height]);
    
    % Allocate its CUDA backing memory:
    GPUallocVector(gpu);
end

% Retrieve CUDA memory pointer to it:
gpuptr = getPtr(gpu);
if gpuptr == 0
    error('Memory allocation on GPU failed!');
end

% Perform copy of image content from OpenGL texture into CUDA backing store:
memcpyCudaOpenGL(gltexid, gltextarget, gpuptr, nrbytes, direction);

if direction == 0
    % OpenGL -> GPU:
    outObj = gpu;
else
    % GPU -> OpenGL:
    if glObjType == 0
        outObj = texid;
    else
        % TODO:
        outObj = outObj; %#ok<ASGSL>
    end
end

return;

end
