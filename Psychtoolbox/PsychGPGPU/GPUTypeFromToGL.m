function gpu = GPUTypeFromToGL(win, texid, gpu, gpuToGL)
% gpu = GPUTypeFromToGL(win, texid [, gpu])
%
% Create and return 'gpu' variable of type GPUsingle from given 'win'dowhandle and
% 'texid' texture. If optional 'gpu' variable is given, non-empty and of
% compatible type and size for given 'texid', refill it with content of
% 'texid' instead of creating a new 'gpu' variable.
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

if nargin < 2
    error('Missing minimum required arguments "win"dowhandle and "texid".');
end

if nargin < 3
    gpu = [];
end

if nargin < 4 || isempty(gpuToGL)
    direction = 0;
else
    direction = gpuToGL;
end

% Get OpenGL texture handle and texture target of underlying OpenGL texture
% for given Psychtoolbox texture:
[gltexid, gltextarget] = Screen('GetOpenGLTexture', win, texid);

% Retrieve width and height of texture in texels:
[width, height] = Screen('Windowsize', texid);

% Retrieve depth of texture in bytes per texel:
bpp = Screen('Pixelsize', texid) / 8;

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

if isempty(gpu) && (direction > 0)
    error('Empty GPUtype variable provided, but update of texture requested. How is this supposed to work?!?');
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
        
        % This must not happen in CUDA -> OpenGL mode:
        if direction > 0
            error('Incompatible GPUtype variable provided for update of texture. How is this supposed to work?!?');
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

% 'gpu' should contain a new GPUsingle variable which is filled with the
% image content of floating point texture 'texid':
return;

end
