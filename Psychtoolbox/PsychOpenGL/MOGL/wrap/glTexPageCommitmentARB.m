function glTexPageCommitmentARB( target, level, xoffset, yoffset, zoffset, width, height, depth, resident )

% glTexPageCommitmentARB  Interface to OpenGL function glTexPageCommitmentARB
%
% usage:  glTexPageCommitmentARB( target, level, xoffset, yoffset, zoffset, width, height, depth, resident )
%
% C function:  void glTexPageCommitmentARB(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLboolean resident)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glTexPageCommitmentARB', target, level, xoffset, yoffset, zoffset, width, height, depth, resident );

return
