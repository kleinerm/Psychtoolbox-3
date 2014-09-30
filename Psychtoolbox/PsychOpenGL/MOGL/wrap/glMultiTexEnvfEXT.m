function glMultiTexEnvfEXT( texunit, target, pname, param )

% glMultiTexEnvfEXT  Interface to OpenGL function glMultiTexEnvfEXT
%
% usage:  glMultiTexEnvfEXT( texunit, target, pname, param )
%
% C function:  void glMultiTexEnvfEXT(GLenum texunit, GLenum target, GLenum pname, GLfloat param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexEnvfEXT', texunit, target, pname, param );

return
