function table = glGetColorTableSGI( target, format, type )

% glGetColorTableSGI  Interface to OpenGL function glGetColorTableSGI
%
% usage:  table = glGetColorTableSGI( target, format, type )
%
% C function:  void glGetColorTableSGI(GLenum target, GLenum format, GLenum type, void* table)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

table = (0);

moglcore( 'glGetColorTableSGI', target, format, type, table );

return
