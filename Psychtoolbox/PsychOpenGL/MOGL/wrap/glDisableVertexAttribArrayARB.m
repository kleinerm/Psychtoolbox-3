function glDisableVertexAttribArrayARB( index )

% glDisableVertexAttribArrayARB  Interface to OpenGL function glDisableVertexAttribArrayARB
%
% usage:  glDisableVertexAttribArrayARB( index )
%
% C function:  void glDisableVertexAttribArrayARB(GLuint index)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glDisableVertexAttribArrayARB', index );

return
