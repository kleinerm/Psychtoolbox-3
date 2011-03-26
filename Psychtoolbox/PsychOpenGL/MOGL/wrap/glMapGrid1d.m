function glMapGrid1d( un, u1, u2 )

% glMapGrid1d  Interface to OpenGL function glMapGrid1d
%
% usage:  glMapGrid1d( un, u1, u2 )
%
% C function:  void glMapGrid1d(GLint un, GLdouble u1, GLdouble u2)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glMapGrid1d', un, u1, u2 );

return
