function glNormalStream3svATI( stream, coords )

% glNormalStream3svATI  Interface to OpenGL function glNormalStream3svATI
%
% usage:  glNormalStream3svATI( stream, coords )
%
% C function:  void glNormalStream3svATI(GLenum stream, const GLshort* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glNormalStream3svATI', stream, int16(coords) );

return
