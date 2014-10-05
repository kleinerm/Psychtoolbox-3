function glDeleteTransformFeedbacksNV( n, ids )

% glDeleteTransformFeedbacksNV  Interface to OpenGL function glDeleteTransformFeedbacksNV
%
% usage:  glDeleteTransformFeedbacksNV( n, ids )
%
% C function:  void glDeleteTransformFeedbacksNV(GLsizei n, const GLuint* ids)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDeleteTransformFeedbacksNV', n, uint32(ids) );

return
