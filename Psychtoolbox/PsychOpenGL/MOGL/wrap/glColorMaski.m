function glColorMaski( index, r, g, b, a )

% glColorMaski  Interface to OpenGL function glColorMaski
%
% usage:  glColorMaski( index, r, g, b, a )
%
% C function:  void glColorMaski(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glColorMaski', index, r, g, b, a );

return
