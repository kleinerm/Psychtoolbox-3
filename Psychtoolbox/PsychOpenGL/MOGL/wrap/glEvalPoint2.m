function glEvalPoint2( i, j )

% glEvalPoint2  Interface to OpenGL function glEvalPoint2
%
% usage:  glEvalPoint2( i, j )
%
% C function:  void glEvalPoint2(GLint i, GLint j)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glEvalPoint2', i, j );

return
