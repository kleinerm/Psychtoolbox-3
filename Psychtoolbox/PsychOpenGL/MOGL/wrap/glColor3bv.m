function glColor3bv( v )

% glColor3bv  Interface to OpenGL function glColor3bv
%
% usage:  glColor3bv( v )
%
% C function:  void glColor3bv(const GLbyte* v)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glColor3bv', int8(v) );

return
