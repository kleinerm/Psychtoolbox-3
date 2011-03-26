function glNormal3bv( v )

% glNormal3bv  Interface to OpenGL function glNormal3bv
%
% usage:  glNormal3bv( v )
%
% C function:  void glNormal3bv(const GLbyte* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glNormal3bv', int8(v) );

return
