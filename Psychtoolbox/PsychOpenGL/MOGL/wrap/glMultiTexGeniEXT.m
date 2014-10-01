function glMultiTexGeniEXT( texunit, coord, pname, param )

% glMultiTexGeniEXT  Interface to OpenGL function glMultiTexGeniEXT
%
% usage:  glMultiTexGeniEXT( texunit, coord, pname, param )
%
% C function:  void glMultiTexGeniEXT(GLenum texunit, GLenum coord, GLenum pname, GLint param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexGeniEXT', texunit, coord, pname, param );

return
