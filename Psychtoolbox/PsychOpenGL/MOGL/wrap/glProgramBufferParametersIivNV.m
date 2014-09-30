function glProgramBufferParametersIivNV( target, bindingIndex, wordIndex, count, params )

% glProgramBufferParametersIivNV  Interface to OpenGL function glProgramBufferParametersIivNV
%
% usage:  glProgramBufferParametersIivNV( target, bindingIndex, wordIndex, count, params )
%
% C function:  void glProgramBufferParametersIivNV(GLenum target, GLuint bindingIndex, GLuint wordIndex, GLsizei count, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glProgramBufferParametersIivNV', target, bindingIndex, wordIndex, count, int32(params) );

return
