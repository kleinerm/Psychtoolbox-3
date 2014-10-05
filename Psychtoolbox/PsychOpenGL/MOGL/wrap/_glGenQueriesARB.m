function ids = glGenQueriesARB( n )

% glGenQueriesARB  Interface to OpenGL function glGenQueriesARB
%
% usage:  ids = glGenQueriesARB( n )
%
% C function:  void glGenQueriesARB(GLsizei n, GLuint* ids)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

ids = uint32(0);

moglcore( 'glGenQueriesARB', n, ids );

return
