function gloperator = CreateGLProcessingOperatorFromShader(windowPtr, shaderhandle, opname, imagingmode)
% gloperator = CreateGLProcessingOperatorFromShader(windowPtr, shaderhandle [, opname] [, imagingmode])
%
% Loads a GLSL shader definition file, converts it into an image processing
% operator for use with the Screen('TransformTexture') function and returns
% a handle to it. The operator should be destroyed via Screen('Close',
% operator); when its not needed anymore. It gets automatically disposed
% when its associated onscreen window is closed.
%

% History:
% 16.4.2007 Written (MK).

if nargin < 2 || isempty(windowPtr) || isempty(shaderhandle)
    error('You must provide a valid ''windowPtr'' window handle and ''shaderhandle''!');
end

if nargin < 3 || isempty(opname)
    opname = 'GL imaging operator without name.';
end

if nargin < 4 || isempty(imagingmode)
    imagingmode = 0;
end

gloperator = Screen('OpenProxy', windowPtr, imagingmode);
Screen('HookFunction', gloperator, 'PrependShader', 'UserDefinedBlit', opname, shaderhandle);
Screen('HookFunction', gloperator, 'Enable', 'UserDefinedBlit');

return
