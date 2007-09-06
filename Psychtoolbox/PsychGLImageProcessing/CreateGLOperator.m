function gloperator = CreateGLOperator(windowPtr, imagingmode, shaderhandle, opname, varargin)
% gloperator = CreateGLOperator(windowPtr [, imagingmode] [, shaderhandle] [, opname] [,..shader options])
%
% Creates an image processing operator for use with the Screen('TransformTexture')
% function (to be passed as 'transformProxy' argument) and returns
% a handle 'gloperator' to it. The operator should be destroyed via Screen('Close', gloperator);
% when its not needed anymore. It gets automatically disposed when its associated onscreen window is closed.
%
% 'windowPtr' is the window handle of the parent onscreen window.
% 'imagingmode' (optional) to set a specifc combination of imaging flags
% that affect how this operator works. Most useful flags are:
% kPsychNeedDualPass if this operator should contain exactly two
% operations, e.g., if it defines a separable convolution which consists of
% two 1-D convolution passes, or kPsychNeedMultiPass if this operator should
% contain more than two operations. Also useful are kPsychNeed16BPCFloat or
% kPsychNeed32BPCFloat if you expect results of the operator to be signed
% or to require floating point resolution. You can use 'mor' to combine
% multiple of these flags, e.g., mor(kPsychNeedDualPass,kPsychNeed32BPCFloat)
% to create an operator suitable for high precision processing with
% possibly signed results and two processing passes.
%
% If you want to immediately assign a single image processing operation,
% you can do so by assigning an OpenGL GLSL shader 'shaderhandle'. You can
% give this shader an unique name 'opname' (for debugging purpose) and
% provide possible additional arguments for it.
%
% If you want to add multiple operations to the operator, you can use the
% AddToGLOperator() command.
%
% GLOperators can also get assigned to the builtin stimulus post-processing
% pipeline if they should affect all created visual stimuli. See 'help
% PsychImaging' - the section about 'AddGLOperator'
%

% History:
% 16.4.2007 Written (MK).

if nargin < 1 || isempty(windowPtr) || Screen('WindowKind', windowPtr)~=1
    error('You must provide a valid ''windowPtr'' onscreen window handle!');
end

if nargin < 2 || isempty(imagingmode)
    imagingmode = 0;
end

if nargin < 3 || isempty(shaderhandle)
    shaderhandle = 0;
end

if nargin < 4 || isempty(opname)
    opname = 'GL operator slot without name.';
end

% Create a proxy window with requested imagingmode:
gloperator = Screen('OpenProxy', windowPtr, imagingmode);

% Enable its processing chain for user-defined blits:
Screen('HookFunction', gloperator, 'Enable', 'UserDefinedBlit');

% Add a provided shader as first slot, if any:
if shaderhandle > 0
    if length(varargin) == 0
        Screen('HookFunction', gloperator, 'AppendShader', 'UserDefinedBlit', opname, shaderhandle);
    else
        AddToGLOperator(gloperator, opname, shaderhandle, varargin{:});
    end
end

return
