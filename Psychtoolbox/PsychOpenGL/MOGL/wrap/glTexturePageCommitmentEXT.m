function glTexturePageCommitmentEXT( texture, level, xoffset, yoffset, zoffset, width, height, depth, resident )

% glTexturePageCommitmentEXT  Interface to OpenGL function glTexturePageCommitmentEXT
%
% usage:  glTexturePageCommitmentEXT( texture, level, xoffset, yoffset, zoffset, width, height, depth, resident )
%
% C function:  void glTexturePageCommitmentEXT(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLboolean resident)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glTexturePageCommitmentEXT', texture, level, xoffset, yoffset, zoffset, width, height, depth, resident );

return
