function glProgramBufferParametersfvNV( target, bindingIndex, wordIndex, count, params )

% glProgramBufferParametersfvNV  Interface to OpenGL function glProgramBufferParametersfvNV
%
% usage:  glProgramBufferParametersfvNV( target, bindingIndex, wordIndex, count, params )
%
% C function:  void glProgramBufferParametersfvNV(GLenum target, GLuint bindingIndex, GLuint wordIndex, GLsizei count, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glProgramBufferParametersfvNV', target, bindingIndex, wordIndex, count, single(params) );

return
