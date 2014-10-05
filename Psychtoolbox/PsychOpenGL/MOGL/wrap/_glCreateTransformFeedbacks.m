function ids = glCreateTransformFeedbacks( n )

% glCreateTransformFeedbacks  Interface to OpenGL function glCreateTransformFeedbacks
%
% usage:  ids = glCreateTransformFeedbacks( n )
%
% C function:  void glCreateTransformFeedbacks(GLsizei n, GLuint* ids)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

ids = uint32(0);

moglcore( 'glCreateTransformFeedbacks', n, ids );

return
