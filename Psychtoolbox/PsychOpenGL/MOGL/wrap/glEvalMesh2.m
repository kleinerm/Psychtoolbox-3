function glEvalMesh2( mode, i1, i2, j1, j2 )

% glEvalMesh2  Interface to OpenGL function glEvalMesh2
%
% usage:  glEvalMesh2( mode, i1, i2, j1, j2 )
%
% C function:  void glEvalMesh2(GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glEvalMesh2', mode, i1, i2, j1, j2 );

return
