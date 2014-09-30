function glDrawTextureNV( texture, sampler, x0, y0, x1, y1, z, s0, t0, s1, t1 )

% glDrawTextureNV  Interface to OpenGL function glDrawTextureNV
%
% usage:  glDrawTextureNV( texture, sampler, x0, y0, x1, y1, z, s0, t0, s1, t1 )
%
% C function:  void glDrawTextureNV(GLuint texture, GLuint sampler, GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1, GLfloat z, GLfloat s0, GLfloat t0, GLfloat s1, GLfloat t1)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=11,
    error('invalid number of arguments');
end

moglcore( 'glDrawTextureNV', texture, sampler, x0, y0, x1, y1, z, s0, t0, s1, t1 );

return
