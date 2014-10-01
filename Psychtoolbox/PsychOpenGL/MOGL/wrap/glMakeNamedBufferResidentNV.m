function glMakeNamedBufferResidentNV( buffer, access )

% glMakeNamedBufferResidentNV  Interface to OpenGL function glMakeNamedBufferResidentNV
%
% usage:  glMakeNamedBufferResidentNV( buffer, access )
%
% C function:  void glMakeNamedBufferResidentNV(GLuint buffer, GLenum access)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMakeNamedBufferResidentNV', buffer, access );

return
