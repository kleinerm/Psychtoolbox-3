function params = glGetDoublei_vEXT( pname, index )

% glGetDoublei_vEXT  Interface to OpenGL function glGetDoublei_vEXT
%
% usage:  params = glGetDoublei_vEXT( pname, index )
%
% C function:  void glGetDoublei_vEXT(GLenum pname, GLuint index, GLdouble* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = double(0);

moglcore( 'glGetDoublei_vEXT', pname, index, params );

return
