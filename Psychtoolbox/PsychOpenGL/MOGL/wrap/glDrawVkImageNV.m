function glDrawVkImageNV( vkImage, sampler, x0, y0, x1, y1, z, s0, t0, s1, t1 )

% glDrawVkImageNV  Interface to OpenGL function glDrawVkImageNV
%
% usage:  glDrawVkImageNV( vkImage, sampler, x0, y0, x1, y1, z, s0, t0, s1, t1 )
%
% C function:  void glDrawVkImageNV(GLuint64 vkImage, GLuint sampler, GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1, GLfloat z, GLfloat s0, GLfloat t0, GLfloat s1, GLfloat t1)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=11,
    error('invalid number of arguments');
end

moglcore( 'glDrawVkImageNV', uint64(vkImage), sampler, x0, y0, x1, y1, z, s0, t0, s1, t1 );

return
