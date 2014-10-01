function glMultiTexBufferEXT( texunit, target, internalformat, buffer )

% glMultiTexBufferEXT  Interface to OpenGL function glMultiTexBufferEXT
%
% usage:  glMultiTexBufferEXT( texunit, target, internalformat, buffer )
%
% C function:  void glMultiTexBufferEXT(GLenum texunit, GLenum target, GLenum internalformat, GLuint buffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexBufferEXT', texunit, target, internalformat, buffer );

return
