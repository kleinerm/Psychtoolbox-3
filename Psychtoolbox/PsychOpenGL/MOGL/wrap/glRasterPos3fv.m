function glRasterPos3fv( v )

% glRasterPos3fv  Interface to OpenGL function glRasterPos3fv
%
% usage:  glRasterPos3fv( v )
%
% C function:  void glRasterPos3fv(const GLfloat* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glRasterPos3fv', single(v) );

return
