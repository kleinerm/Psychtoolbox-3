function AddToGLOperator(gloperator, opname, shaderhandle, varargin)
% AddToGLOperator(gloperator, opname, shaderhandle [, ...optional arguments])
%
% Add additional GLSL image processing shaders to GL operator 'gloperator'.
%
% 'opname' is the name of the to be added shader -- purely for debug
% purpose.
%
% 'shaderhandle' is the GLSL handle of the shader to add.
%

if nargin < 1 || isempty(gloperator) || Screen('WindowKind', gloperator)~=4
    error('You must provide the handle of a valid GL imaging operator ''gloperator''!');
end

if nargin < 2 || isempty(opname)
    opname = 'GL operator slot without name.';
end

if nargin < 3 || isempty(shaderhandle)
    error('You must provide a valid GLSL shader handle ''shaderhandle''!');
end

if nargin >= 4
    if length(varargin)>=1
        blittercfg = varargin{1};
    else
        blittercfg = '';
    end
    
    if length(varargin)>=2
        luttexid = varargin{2};
    else
        luttexid = 0;
    end
    
    
else
    blittercfg = '';
    luttexid = 0;
end

% Add shader to user defined blit chain of the proxy:
Screen('HookFunction', gloperator, 'AppendShader', 'UserDefinedBlit', opname, shaderhandle, blittercfg, luttexid);

% Count number of slots:
count = CountSlotsInGLOperator(gloperator);
if count == 2
    % Change operator to be dualpass capable:
    Screen('HookFunction', gloperator, 'ImagingMode', mor(kPsychNeedDualPass, Screen('HookFunction', gloperator, 'ImagingMode')));
end

if count > 2
    % Change operator to be dualpass capable:
    Screen('HookFunction', gloperator, 'ImagingMode', mor(kPsychNeedMultiPass, Screen('HookFunction', gloperator, 'ImagingMode')));
end

% Done.
return;
