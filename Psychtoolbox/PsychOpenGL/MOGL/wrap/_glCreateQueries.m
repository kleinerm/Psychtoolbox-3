function ids = glCreateQueries( target, n )

% glCreateQueries  Interface to OpenGL function glCreateQueries
%
% usage:  ids = glCreateQueries( target, n )
%
% C function:  void glCreateQueries(GLenum target, GLsizei n, GLuint* ids)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

ids = uint32(0);

moglcore( 'glCreateQueries', target, n, ids );

return
