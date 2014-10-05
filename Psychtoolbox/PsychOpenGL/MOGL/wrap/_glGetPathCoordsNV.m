function coords = glGetPathCoordsNV( path )

% glGetPathCoordsNV  Interface to OpenGL function glGetPathCoordsNV
%
% usage:  coords = glGetPathCoordsNV( path )
%
% C function:  void glGetPathCoordsNV(GLuint path, GLfloat* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

coords = single(0);

moglcore( 'glGetPathCoordsNV', path, coords );

return
