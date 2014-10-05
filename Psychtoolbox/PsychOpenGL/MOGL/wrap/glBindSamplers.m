function glBindSamplers( first, count, samplers )

% glBindSamplers  Interface to OpenGL function glBindSamplers
%
% usage:  glBindSamplers( first, count, samplers )
%
% C function:  void glBindSamplers(GLuint first, GLsizei count, const GLuint* samplers)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glBindSamplers', first, count, uint32(samplers) );

return
