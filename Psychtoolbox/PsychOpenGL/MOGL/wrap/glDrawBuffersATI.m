function glDrawBuffersATI( n, bufs )

% glDrawBuffersATI  Interface to OpenGL function glDrawBuffersATI
%
% usage:  glDrawBuffersATI( n, bufs )
%
% C function:  void glDrawBuffersATI(GLsizei n, const GLenum* bufs)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDrawBuffersATI', n, uint32(bufs) );

return
