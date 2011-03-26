function glRasterPos2iv( v )

% glRasterPos2iv  Interface to OpenGL function glRasterPos2iv
%
% usage:  glRasterPos2iv( v )
%
% C function:  void glRasterPos2iv(const GLint* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glRasterPos2iv', int32(v) );

return
