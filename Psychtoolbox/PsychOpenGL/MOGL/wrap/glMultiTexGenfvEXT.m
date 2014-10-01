function glMultiTexGenfvEXT( texunit, coord, pname, params )

% glMultiTexGenfvEXT  Interface to OpenGL function glMultiTexGenfvEXT
%
% usage:  glMultiTexGenfvEXT( texunit, coord, pname, params )
%
% C function:  void glMultiTexGenfvEXT(GLenum texunit, GLenum coord, GLenum pname, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexGenfvEXT', texunit, coord, pname, single(params) );

return
