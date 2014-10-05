function param = glGetTransformFeedbacki_v( xfb, pname, index )

% glGetTransformFeedbacki_v  Interface to OpenGL function glGetTransformFeedbacki_v
%
% usage:  param = glGetTransformFeedbacki_v( xfb, pname, index )
%
% C function:  void glGetTransformFeedbacki_v(GLuint xfb, GLenum pname, GLuint index, GLint* param)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

param = int32(0);

moglcore( 'glGetTransformFeedbacki_v', xfb, pname, index, param );

return
