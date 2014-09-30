function glBindProgramNV( target, id )

% glBindProgramNV  Interface to OpenGL function glBindProgramNV
%
% usage:  glBindProgramNV( target, id )
%
% C function:  void glBindProgramNV(GLenum target, GLuint id)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glBindProgramNV', target, id );

return
