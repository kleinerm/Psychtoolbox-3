function glDeleteQueriesARB( n, ids )

% glDeleteQueriesARB  Interface to OpenGL function glDeleteQueriesARB
%
% usage:  glDeleteQueriesARB( n, ids )
%
% C function:  void glDeleteQueriesARB(GLsizei n, const GLuint* ids)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDeleteQueriesARB', n, uint32(ids) );

return
