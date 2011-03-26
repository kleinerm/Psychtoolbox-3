function glDeleteQueries( n, ids )

% glDeleteQueries  Interface to OpenGL function glDeleteQueries
%
% usage:  glDeleteQueries( n, ids )
%
% C function:  void glDeleteQueries(GLsizei n, const GLuint* ids)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDeleteQueries', n, uint32(ids) );

return
