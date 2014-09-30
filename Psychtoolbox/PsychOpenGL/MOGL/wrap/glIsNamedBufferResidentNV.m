function r = glIsNamedBufferResidentNV( buffer )

% glIsNamedBufferResidentNV  Interface to OpenGL function glIsNamedBufferResidentNV
%
% usage:  r = glIsNamedBufferResidentNV( buffer )
%
% C function:  GLboolean glIsNamedBufferResidentNV(GLuint buffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsNamedBufferResidentNV', buffer );

return
