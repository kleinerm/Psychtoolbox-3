function glMultiTexEnvivEXT( texunit, target, pname, params )

% glMultiTexEnvivEXT  Interface to OpenGL function glMultiTexEnvivEXT
%
% usage:  glMultiTexEnvivEXT( texunit, target, pname, params )
%
% C function:  void glMultiTexEnvivEXT(GLenum texunit, GLenum target, GLenum pname, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexEnvivEXT', texunit, target, pname, int32(params) );

return
