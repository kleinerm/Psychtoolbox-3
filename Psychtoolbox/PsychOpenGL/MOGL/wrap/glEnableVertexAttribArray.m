function glEnableVertexAttribArray( index )

% glEnableVertexAttribArray  Interface to OpenGL function glEnableVertexAttribArray
%
% usage:  glEnableVertexAttribArray( index )
%
% C function:  void glEnableVertexAttribArray(GLuint index)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glEnableVertexAttribArray', index );

return
