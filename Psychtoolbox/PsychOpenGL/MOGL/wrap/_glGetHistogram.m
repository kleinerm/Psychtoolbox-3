function values = glGetHistogram( target, reset, format, type )

% glGetHistogram  Interface to OpenGL function glGetHistogram
%
% usage:  values = glGetHistogram( target, reset, format, type )
%
% C function:  void glGetHistogram(GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid* values)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=4,
    error('invalid number of arguments');
end

values = (0);

moglcore( 'glGetHistogram', target, reset, format, type, values );

return
