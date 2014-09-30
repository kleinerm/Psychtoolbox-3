function glNamedBufferSubData( buffer, offset, size, data )

% glNamedBufferSubData  Interface to OpenGL function glNamedBufferSubData
%
% usage:  glNamedBufferSubData( buffer, offset, size, data )
%
% C function:  void glNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizei size, const void* data)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNamedBufferSubData', buffer, offset, size, data );

return
