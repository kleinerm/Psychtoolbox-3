function glSubpixelPrecisionBiasNV( xbits, ybits )

% glSubpixelPrecisionBiasNV  Interface to OpenGL function glSubpixelPrecisionBiasNV
%
% usage:  glSubpixelPrecisionBiasNV( xbits, ybits )
%
% C function:  void glSubpixelPrecisionBiasNV(GLuint xbits, GLuint ybits)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glSubpixelPrecisionBiasNV', xbits, ybits );

return
