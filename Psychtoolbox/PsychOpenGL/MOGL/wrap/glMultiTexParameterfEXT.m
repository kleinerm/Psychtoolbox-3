function glMultiTexParameterfEXT( texunit, target, pname, param )

% glMultiTexParameterfEXT  Interface to OpenGL function glMultiTexParameterfEXT
%
% usage:  glMultiTexParameterfEXT( texunit, target, pname, param )
%
% C function:  void glMultiTexParameterfEXT(GLenum texunit, GLenum target, GLenum pname, GLfloat param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexParameterfEXT', texunit, target, pname, param );

return
