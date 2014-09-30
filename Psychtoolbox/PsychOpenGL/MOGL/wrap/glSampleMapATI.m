function glSampleMapATI( dst, interp, swizzle )

% glSampleMapATI  Interface to OpenGL function glSampleMapATI
%
% usage:  glSampleMapATI( dst, interp, swizzle )
%
% C function:  void glSampleMapATI(GLuint dst, GLuint interp, GLenum swizzle)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glSampleMapATI', dst, interp, swizzle );

return
