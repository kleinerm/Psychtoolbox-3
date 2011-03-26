function glColor3sv( v )

% glColor3sv  Interface to OpenGL function glColor3sv
%
% usage:  glColor3sv( v )
%
% C function:  void glColor3sv(const GLshort* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glColor3sv', int16(v) );

return
