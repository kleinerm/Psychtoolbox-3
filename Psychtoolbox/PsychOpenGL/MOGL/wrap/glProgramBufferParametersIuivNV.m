function glProgramBufferParametersIuivNV( target, bindingIndex, wordIndex, count, params )

% glProgramBufferParametersIuivNV  Interface to OpenGL function glProgramBufferParametersIuivNV
%
% usage:  glProgramBufferParametersIuivNV( target, bindingIndex, wordIndex, count, params )
%
% C function:  void glProgramBufferParametersIuivNV(GLenum target, GLuint bindingIndex, GLuint wordIndex, GLsizei count, const GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glProgramBufferParametersIuivNV', target, bindingIndex, wordIndex, count, uint32(params) );

return
