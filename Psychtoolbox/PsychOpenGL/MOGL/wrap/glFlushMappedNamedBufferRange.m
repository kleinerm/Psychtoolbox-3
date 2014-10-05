function glFlushMappedNamedBufferRange( buffer, offset, length )

% glFlushMappedNamedBufferRange  Interface to OpenGL function glFlushMappedNamedBufferRange
%
% usage:  glFlushMappedNamedBufferRange( buffer, offset, length )
%
% C function:  void glFlushMappedNamedBufferRange(GLuint buffer, GLintptr offset, GLsizei length)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glFlushMappedNamedBufferRange', buffer, offset, length );

return
