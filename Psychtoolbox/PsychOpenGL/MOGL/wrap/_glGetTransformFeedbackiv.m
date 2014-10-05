function param = glGetTransformFeedbackiv( xfb, pname )

% glGetTransformFeedbackiv  Interface to OpenGL function glGetTransformFeedbackiv
%
% usage:  param = glGetTransformFeedbackiv( xfb, pname )
%
% C function:  void glGetTransformFeedbackiv(GLuint xfb, GLenum pname, GLint* param)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

param = int32(0);

moglcore( 'glGetTransformFeedbackiv', xfb, pname, param );

return
