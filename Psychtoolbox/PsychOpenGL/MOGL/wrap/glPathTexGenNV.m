function glPathTexGenNV( texCoordSet, genMode, components, coeffs )

% glPathTexGenNV  Interface to OpenGL function glPathTexGenNV
%
% usage:  glPathTexGenNV( texCoordSet, genMode, components, coeffs )
%
% C function:  void glPathTexGenNV(GLenum texCoordSet, GLenum genMode, GLint components, const GLfloat* coeffs)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glPathTexGenNV', texCoordSet, genMode, components, single(coeffs) );

return
