function glRasterPos4iv( v )

% glRasterPos4iv  Interface to OpenGL function glRasterPos4iv
%
% usage:  glRasterPos4iv( v )
%
% C function:  void glRasterPos4iv(const GLint* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glRasterPos4iv', int32(v) );

return
