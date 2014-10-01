function glMultiTexGendEXT( texunit, coord, pname, param )

% glMultiTexGendEXT  Interface to OpenGL function glMultiTexGendEXT
%
% usage:  glMultiTexGendEXT( texunit, coord, pname, param )
%
% C function:  void glMultiTexGendEXT(GLenum texunit, GLenum coord, GLenum pname, GLdouble param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexGendEXT', texunit, coord, pname, param );

return
