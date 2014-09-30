function glExtractComponentEXT( res, src, num )

% glExtractComponentEXT  Interface to OpenGL function glExtractComponentEXT
%
% usage:  glExtractComponentEXT( res, src, num )
%
% C function:  void glExtractComponentEXT(GLuint res, GLuint src, GLuint num)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glExtractComponentEXT', res, src, num );

return
