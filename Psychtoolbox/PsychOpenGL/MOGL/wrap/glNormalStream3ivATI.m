function glNormalStream3ivATI( stream, coords )

% glNormalStream3ivATI  Interface to OpenGL function glNormalStream3ivATI
%
% usage:  glNormalStream3ivATI( stream, coords )
%
% C function:  void glNormalStream3ivATI(GLenum stream, const GLint* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glNormalStream3ivATI', stream, int32(coords) );

return
