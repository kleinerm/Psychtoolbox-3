function r = glTestFenceNV( fence )

% glTestFenceNV  Interface to OpenGL function glTestFenceNV
%
% usage:  r = glTestFenceNV( fence )
%
% C function:  GLboolean glTestFenceNV(GLuint fence)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glTestFenceNV', fence );

return
