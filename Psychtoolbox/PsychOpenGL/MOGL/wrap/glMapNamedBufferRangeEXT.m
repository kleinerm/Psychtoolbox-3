function glMapNamedBufferRangeEXT( buffer, offset, length, access )

% glMapNamedBufferRangeEXT  Interface to OpenGL function glMapNamedBufferRangeEXT
%
% usage:  glMapNamedBufferRangeEXT( buffer, offset, length, access )
%
% C function:  void* glMapNamedBufferRangeEXT(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMapNamedBufferRangeEXT', buffer, offset, length, access );

return
