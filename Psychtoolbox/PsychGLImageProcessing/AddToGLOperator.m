function AddToGLOperator(gloperator, opname, shaderhandle, varargin)
% AddToGLOperator(gloperator, opname, shaderhandle [, blittercfg] [, luttexid])
%
% Add additional GLSL image processing shaders to GL operator 'gloperator'.
%
% 'opname' is the name of the to be added shader -- purely for debug
% purpose.
%
% 'shaderhandle' is the GLSL handle of the shader to add.
%
% 'blittercfg' is the optional blitter configuration string with special
% parameters.
%
% 'luttexid' is the optional numeric OpenGL handle of a texture for use as
% lookup-table.

% History:
% 08/11/07 Written (MK).

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

% Count number of slots:
count = CountSlotsInGLOperator(gloperator);
if count > 0
    Screen('HookFunction', gloperator, 'AppendBuiltin', 'UserDefinedBlit', 'Builtin:FlipFBOs', '');
end

if count == 1
    % Change operator to be dual-pass capable:
    Screen('HookFunction', gloperator, 'ImagingMode', mor(kPsychNeedDualPass, Screen('HookFunction', gloperator, 'ImagingMode')));
end

if count > 1
    % Change operator to be multi-pass capable:
    Screen('HookFunction', gloperator, 'ImagingMode', mor(kPsychNeedMultiPass, Screen('HookFunction', gloperator, 'ImagingMode')));
end

% Add shader to user defined blit chain of the proxy:
Screen('HookFunction', gloperator, 'AppendShader', 'UserDefinedBlit', opname, shaderhandle, blittercfg, luttexid);

% Done.
return;
