function glProgramLocalParameters4fvEXT( target, index, count, params )

% glProgramLocalParameters4fvEXT  Interface to OpenGL function glProgramLocalParameters4fvEXT
%
% usage:  glProgramLocalParameters4fvEXT( target, index, count, params )
%
% C function:  void glProgramLocalParameters4fvEXT(GLenum target, GLuint index, GLsizei count, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramLocalParameters4fvEXT', target, index, count, single(params) );

return
