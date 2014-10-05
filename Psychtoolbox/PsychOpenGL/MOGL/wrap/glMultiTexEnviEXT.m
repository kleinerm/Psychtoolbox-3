function glMultiTexEnviEXT( texunit, target, pname, param )

% glMultiTexEnviEXT  Interface to OpenGL function glMultiTexEnviEXT
%
% usage:  glMultiTexEnviEXT( texunit, target, pname, param )
%
% C function:  void glMultiTexEnviEXT(GLenum texunit, GLenum target, GLenum pname, GLint param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexEnviEXT', texunit, target, pname, param );

return
