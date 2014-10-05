function glEnableVertexAttribArrayARB( index )

% glEnableVertexAttribArrayARB  Interface to OpenGL function glEnableVertexAttribArrayARB
%
% usage:  glEnableVertexAttribArrayARB( index )
%
% C function:  void glEnableVertexAttribArrayARB(GLuint index)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glEnableVertexAttribArrayARB', index );

return
