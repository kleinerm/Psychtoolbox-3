function glEvalCoord1fv( u )

% glEvalCoord1fv  Interface to OpenGL function glEvalCoord1fv
%
% usage:  glEvalCoord1fv( u )
%
% C function:  void glEvalCoord1fv(const GLfloat* u)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glEvalCoord1fv', single(u) );

return
