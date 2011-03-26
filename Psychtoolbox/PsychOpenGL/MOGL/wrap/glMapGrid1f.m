function glMapGrid1f( un, u1, u2 )

% glMapGrid1f  Interface to OpenGL function glMapGrid1f
%
% usage:  glMapGrid1f( un, u1, u2 )
%
% C function:  void glMapGrid1f(GLint un, GLfloat u1, GLfloat u2)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glMapGrid1f', un, u1, u2 );

return
