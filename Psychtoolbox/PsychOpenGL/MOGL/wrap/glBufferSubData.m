function glBufferSubData( target, ptr, ptr, data )

% glBufferSubData  Interface to OpenGL function glBufferSubData
%
% usage:  glBufferSubData( target, ptr, ptr, data )
%
% C function:  void glBufferSubData(GLenum target, GLint ptr, GLsizei ptr, const GLvoid* data)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glBufferSubData', target, ptr, ptr, data );

return
