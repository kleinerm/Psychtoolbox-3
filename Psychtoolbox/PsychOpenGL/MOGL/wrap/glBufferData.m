function glBufferData( target, ptr, data, usage )

% glBufferData  Interface to OpenGL function glBufferData
%
% usage:  glBufferData( target, ptr, data, usage )
%
% C function:  void glBufferData(GLenum target, GLsizei ptr, const GLvoid* data, GLenum usage)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---protected---

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glBufferData', target, ptr, data, usage );

return
