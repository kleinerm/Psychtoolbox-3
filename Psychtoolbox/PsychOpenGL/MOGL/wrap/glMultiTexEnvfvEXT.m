function glMultiTexEnvfvEXT( texunit, target, pname, params )

% glMultiTexEnvfvEXT  Interface to OpenGL function glMultiTexEnvfvEXT
%
% usage:  glMultiTexEnvfvEXT( texunit, target, pname, params )
%
% C function:  void glMultiTexEnvfvEXT(GLenum texunit, GLenum target, GLenum pname, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexEnvfvEXT', texunit, target, pname, single(params) );

return
