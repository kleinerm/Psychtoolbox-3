function r = glTestFenceAPPLE( fence )

% glTestFenceAPPLE  Interface to OpenGL function glTestFenceAPPLE
%
% usage:  r = glTestFenceAPPLE( fence )
%
% C function:  GLboolean glTestFenceAPPLE(GLuint fence)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glTestFenceAPPLE', fence );

return
