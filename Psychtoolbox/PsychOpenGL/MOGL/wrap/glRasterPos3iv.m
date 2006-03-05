function glRasterPos3iv( v )

% glRasterPos3iv  Interface to OpenGL function glRasterPos3iv
%
% usage:  glRasterPos3iv( v )
%
% C function:  void glRasterPos3iv(const GLint* v)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glRasterPos3iv', int32(v) );

return
