function glPassTexCoordATI( dst, coord, swizzle )

% glPassTexCoordATI  Interface to OpenGL function glPassTexCoordATI
%
% usage:  glPassTexCoordATI( dst, coord, swizzle )
%
% C function:  void glPassTexCoordATI(GLuint dst, GLuint coord, GLenum swizzle)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glPassTexCoordATI', dst, coord, swizzle );

return
