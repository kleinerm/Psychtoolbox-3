function glTransformFeedbackBufferRange( xfb, index, buffer, offset, size )

% glTransformFeedbackBufferRange  Interface to OpenGL function glTransformFeedbackBufferRange
%
% usage:  glTransformFeedbackBufferRange( xfb, index, buffer, offset, size )
%
% C function:  void glTransformFeedbackBufferRange(GLuint xfb, GLuint index, GLuint buffer, GLintptr offset, GLsizei size)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glTransformFeedbackBufferRange', xfb, index, buffer, offset, size );

return
