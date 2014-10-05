function param = glGetTransformFeedbacki64_v( xfb, pname, index )

% glGetTransformFeedbacki64_v  Interface to OpenGL function glGetTransformFeedbacki64_v
%
% usage:  param = glGetTransformFeedbacki64_v( xfb, pname, index )
%
% C function:  void glGetTransformFeedbacki64_v(GLuint xfb, GLenum pname, GLuint index, GLint64* param)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

param = int64(0);

moglcore( 'glGetTransformFeedbacki64_v', xfb, pname, index, param );

return
