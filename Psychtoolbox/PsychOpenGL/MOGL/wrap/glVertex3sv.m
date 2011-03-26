function glVertex3sv( v )

% glVertex3sv  Interface to OpenGL function glVertex3sv
%
% usage:  glVertex3sv( v )
%
% C function:  void glVertex3sv(const GLshort* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glVertex3sv', int16(v) );

return
