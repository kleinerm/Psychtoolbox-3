function glInsertComponentEXT( res, src, num )

% glInsertComponentEXT  Interface to OpenGL function glInsertComponentEXT
%
% usage:  glInsertComponentEXT( res, src, num )
%
% C function:  void glInsertComponentEXT(GLuint res, GLuint src, GLuint num)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glInsertComponentEXT', res, src, num );

return
