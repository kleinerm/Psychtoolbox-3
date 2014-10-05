function location = glGetTransformFeedbackVaryingNV( program, index )

% glGetTransformFeedbackVaryingNV  Interface to OpenGL function glGetTransformFeedbackVaryingNV
%
% usage:  location = glGetTransformFeedbackVaryingNV( program, index )
%
% C function:  void glGetTransformFeedbackVaryingNV(GLuint program, GLuint index, GLint* location)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=2,
    error('invalid number of arguments');
end

location = int32(0);

moglcore( 'glGetTransformFeedbackVaryingNV', program, index, location );

return
