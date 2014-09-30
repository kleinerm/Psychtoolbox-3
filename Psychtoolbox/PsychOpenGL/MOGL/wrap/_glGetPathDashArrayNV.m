function dashArray = glGetPathDashArrayNV( path )

% glGetPathDashArrayNV  Interface to OpenGL function glGetPathDashArrayNV
%
% usage:  dashArray = glGetPathDashArrayNV( path )
%
% C function:  void glGetPathDashArrayNV(GLuint path, GLfloat* dashArray)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

dashArray = single(0);

moglcore( 'glGetPathDashArrayNV', path, dashArray );

return
