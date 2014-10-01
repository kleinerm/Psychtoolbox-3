function glTransformFeedbackBufferBase( xfb, index, buffer )

% glTransformFeedbackBufferBase  Interface to OpenGL function glTransformFeedbackBufferBase
%
% usage:  glTransformFeedbackBufferBase( xfb, index, buffer )
%
% C function:  void glTransformFeedbackBufferBase(GLuint xfb, GLuint index, GLuint buffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTransformFeedbackBufferBase', xfb, index, buffer );

return
