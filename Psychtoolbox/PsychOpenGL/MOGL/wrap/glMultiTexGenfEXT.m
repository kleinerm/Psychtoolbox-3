function glMultiTexGenfEXT( texunit, coord, pname, param )

% glMultiTexGenfEXT  Interface to OpenGL function glMultiTexGenfEXT
%
% usage:  glMultiTexGenfEXT( texunit, coord, pname, param )
%
% C function:  void glMultiTexGenfEXT(GLenum texunit, GLenum coord, GLenum pname, GLfloat param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexGenfEXT', texunit, coord, pname, param );

return
