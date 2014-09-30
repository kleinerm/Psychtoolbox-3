function data = glGetIntegeri_v( target, index )

% glGetIntegeri_v  Interface to OpenGL function glGetIntegeri_v
%
% usage:  data = glGetIntegeri_v( target, index )
%
% C function:  void glGetIntegeri_v(GLenum target, GLuint index, GLint* data)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

data = int32(0);

moglcore( 'glGetIntegeri_v', target, index, data );

return
