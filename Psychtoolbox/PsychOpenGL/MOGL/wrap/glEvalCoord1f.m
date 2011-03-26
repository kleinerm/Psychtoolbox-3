function glEvalCoord1f( u )

% glEvalCoord1f  Interface to OpenGL function glEvalCoord1f
%
% usage:  glEvalCoord1f( u )
%
% C function:  void glEvalCoord1f(GLfloat u)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glEvalCoord1f', u );

return
