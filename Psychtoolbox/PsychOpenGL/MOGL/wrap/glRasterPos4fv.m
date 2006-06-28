function glRasterPos4fv( v )

% glRasterPos4fv  Interface to OpenGL function glRasterPos4fv
%
% usage:  glRasterPos4fv( v )
%
% C function:  void glRasterPos4fv(const GLfloat* v)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glRasterPos4fv', moglsingle(v) );

return
