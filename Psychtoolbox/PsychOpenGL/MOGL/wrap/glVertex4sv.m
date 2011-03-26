function glVertex4sv( v )

% glVertex4sv  Interface to OpenGL function glVertex4sv
%
% usage:  glVertex4sv( v )
%
% C function:  void glVertex4sv(const GLshort* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glVertex4sv', int16(v) );

return
