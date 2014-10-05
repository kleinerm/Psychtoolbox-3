function glPathColorGenNV( color, genMode, colorFormat, coeffs )

% glPathColorGenNV  Interface to OpenGL function glPathColorGenNV
%
% usage:  glPathColorGenNV( color, genMode, colorFormat, coeffs )
%
% C function:  void glPathColorGenNV(GLenum color, GLenum genMode, GLenum colorFormat, const GLfloat* coeffs)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glPathColorGenNV', color, genMode, colorFormat, single(coeffs) );

return
