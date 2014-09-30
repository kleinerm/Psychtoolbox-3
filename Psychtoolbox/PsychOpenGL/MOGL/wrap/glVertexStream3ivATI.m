function glVertexStream3ivATI( stream, coords )

% glVertexStream3ivATI  Interface to OpenGL function glVertexStream3ivATI
%
% usage:  glVertexStream3ivATI( stream, coords )
%
% C function:  void glVertexStream3ivATI(GLenum stream, const GLint* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream3ivATI', stream, int32(coords) );

return
