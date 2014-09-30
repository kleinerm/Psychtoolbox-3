function ids = glGenTransformFeedbacksNV( n )

% glGenTransformFeedbacksNV  Interface to OpenGL function glGenTransformFeedbacksNV
%
% usage:  ids = glGenTransformFeedbacksNV( n )
%
% C function:  void glGenTransformFeedbacksNV(GLsizei n, GLuint* ids)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

ids = uint32(0);

moglcore( 'glGenTransformFeedbacksNV', n, ids );

return
