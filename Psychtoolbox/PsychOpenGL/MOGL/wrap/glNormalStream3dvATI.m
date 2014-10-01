function glNormalStream3dvATI( stream, coords )

% glNormalStream3dvATI  Interface to OpenGL function glNormalStream3dvATI
%
% usage:  glNormalStream3dvATI( stream, coords )
%
% C function:  void glNormalStream3dvATI(GLenum stream, const GLdouble* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glNormalStream3dvATI', stream, double(coords) );

return
