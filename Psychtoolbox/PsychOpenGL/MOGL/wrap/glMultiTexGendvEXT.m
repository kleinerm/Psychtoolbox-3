function glMultiTexGendvEXT( texunit, coord, pname, params )

% glMultiTexGendvEXT  Interface to OpenGL function glMultiTexGendvEXT
%
% usage:  glMultiTexGendvEXT( texunit, coord, pname, params )
%
% C function:  void glMultiTexGendvEXT(GLenum texunit, GLenum coord, GLenum pname, const GLdouble* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexGendvEXT', texunit, coord, pname, double(params) );

return
