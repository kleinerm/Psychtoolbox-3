function glMapGrid2d( un, u1, u2, vn, v1, v2 )

% glMapGrid2d  Interface to OpenGL function glMapGrid2d
%
% usage:  glMapGrid2d( un, u1, u2, vn, v1, v2 )
%
% C function:  void glMapGrid2d(GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glMapGrid2d', un, u1, u2, vn, v1, v2 );

return
