function r = glIsFenceAPPLE( fence )

% glIsFenceAPPLE  Interface to OpenGL function glIsFenceAPPLE
%
% usage:  r = glIsFenceAPPLE( fence )
%
% C function:  GLboolean glIsFenceAPPLE(GLuint fence)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsFenceAPPLE', fence );

return
