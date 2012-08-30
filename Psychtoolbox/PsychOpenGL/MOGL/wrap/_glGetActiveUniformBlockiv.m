function params = glGetActiveUniformBlockiv( program, uniformBlockIndex, pname )

% glGetActiveUniformBlockiv  Interface to OpenGL function glGetActiveUniformBlockiv
%
% usage:  params = glGetActiveUniformBlockiv( program, uniformBlockIndex, pname )
%
% C function:  void glGetActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params)

% 30-Aug-2012 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=3,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetActiveUniformBlockiv', program, uniformBlockIndex, pname, params );

return
