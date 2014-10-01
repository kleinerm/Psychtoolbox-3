function glProgramEnvParameters4fvEXT( target, index, count, params )

% glProgramEnvParameters4fvEXT  Interface to OpenGL function glProgramEnvParameters4fvEXT
%
% usage:  glProgramEnvParameters4fvEXT( target, index, count, params )
%
% C function:  void glProgramEnvParameters4fvEXT(GLenum target, GLuint index, GLsizei count, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramEnvParameters4fvEXT', target, index, count, single(params) );

return
