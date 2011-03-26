function glRasterPos2fv( v )

% glRasterPos2fv  Interface to OpenGL function glRasterPos2fv
%
% usage:  glRasterPos2fv( v )
%
% C function:  void glRasterPos2fv(const GLfloat* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glRasterPos2fv', single(v) );

return
