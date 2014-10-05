function result = glGetIntegerui64vNV( value )

% glGetIntegerui64vNV  Interface to OpenGL function glGetIntegerui64vNV
%
% usage:  result = glGetIntegerui64vNV( value )
%
% C function:  void glGetIntegerui64vNV(GLenum value, GLuint64EXT* result)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

result = uint64(0);

moglcore( 'glGetIntegerui64vNV', value, result );

return
