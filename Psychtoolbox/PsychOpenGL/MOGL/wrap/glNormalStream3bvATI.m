function glNormalStream3bvATI( stream, coords )

% glNormalStream3bvATI  Interface to OpenGL function glNormalStream3bvATI
%
% usage:  glNormalStream3bvATI( stream, coords )
%
% C function:  void glNormalStream3bvATI(GLenum stream, const GLbyte* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glNormalStream3bvATI', stream, int8(coords) );

return
