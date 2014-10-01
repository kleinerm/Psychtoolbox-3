function glNormalStream3fvATI( stream, coords )

% glNormalStream3fvATI  Interface to OpenGL function glNormalStream3fvATI
%
% usage:  glNormalStream3fvATI( stream, coords )
%
% C function:  void glNormalStream3fvATI(GLenum stream, const GLfloat* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glNormalStream3fvATI', stream, single(coords) );

return
