function r = glIsFenceNV( fence )

% glIsFenceNV  Interface to OpenGL function glIsFenceNV
%
% usage:  r = glIsFenceNV( fence )
%
% C function:  GLboolean glIsFenceNV(GLuint fence)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsFenceNV', fence );

return
