function glClientActiveVertexStreamATI( stream )

% glClientActiveVertexStreamATI  Interface to OpenGL function glClientActiveVertexStreamATI
%
% usage:  glClientActiveVertexStreamATI( stream )
%
% C function:  void glClientActiveVertexStreamATI(GLenum stream)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glClientActiveVertexStreamATI', stream );

return
