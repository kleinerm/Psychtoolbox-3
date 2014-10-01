function glSetMultisamplefvAMD( pname, index, val )

% glSetMultisamplefvAMD  Interface to OpenGL function glSetMultisamplefvAMD
%
% usage:  glSetMultisamplefvAMD( pname, index, val )
%
% C function:  void glSetMultisamplefvAMD(GLenum pname, GLuint index, const GLfloat* val)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glSetMultisamplefvAMD', pname, index, single(val) );

return
