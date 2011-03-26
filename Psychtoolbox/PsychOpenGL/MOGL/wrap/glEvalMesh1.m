function glEvalMesh1( mode, i1, i2 )

% glEvalMesh1  Interface to OpenGL function glEvalMesh1
%
% usage:  glEvalMesh1( mode, i1, i2 )
%
% C function:  void glEvalMesh1(GLenum mode, GLint i1, GLint i2)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glEvalMesh1', mode, i1, i2 );

return
