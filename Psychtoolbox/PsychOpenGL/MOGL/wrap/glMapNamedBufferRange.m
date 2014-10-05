function glMapNamedBufferRange( buffer, offset, length, access )

% glMapNamedBufferRange  Interface to OpenGL function glMapNamedBufferRange
%
% usage:  glMapNamedBufferRange( buffer, offset, length, access )
%
% C function:  void* glMapNamedBufferRange(GLuint buffer, GLintptr offset, GLsizei length, GLbitfield access)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMapNamedBufferRange', buffer, offset, length, access );

return
