function result = glGetIntegerui64i_vNV( value, index )

% glGetIntegerui64i_vNV  Interface to OpenGL function glGetIntegerui64i_vNV
%
% usage:  result = glGetIntegerui64i_vNV( value, index )
%
% C function:  void glGetIntegerui64i_vNV(GLenum value, GLuint index, GLuint64EXT* result)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

result = uint64(0);

moglcore( 'glGetIntegerui64i_vNV', value, index, result );

return
