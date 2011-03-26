function glMapGrid2f( un, u1, u2, vn, v1, v2 )

% glMapGrid2f  Interface to OpenGL function glMapGrid2f
%
% usage:  glMapGrid2f( un, u1, u2, vn, v1, v2 )
%
% C function:  void glMapGrid2f(GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glMapGrid2f', un, u1, u2, vn, v1, v2 );

return
