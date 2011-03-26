function glNormal3sv( v )

% glNormal3sv  Interface to OpenGL function glNormal3sv
%
% usage:  glNormal3sv( v )
%
% C function:  void glNormal3sv(const GLshort* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glNormal3sv', int16(v) );

return
