function glBindBufferBaseNV( target, index, buffer )

% glBindBufferBaseNV  Interface to OpenGL function glBindBufferBaseNV
%
% usage:  glBindBufferBaseNV( target, index, buffer )
%
% C function:  void glBindBufferBaseNV(GLenum target, GLuint index, GLuint buffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glBindBufferBaseNV', target, index, buffer );

return
