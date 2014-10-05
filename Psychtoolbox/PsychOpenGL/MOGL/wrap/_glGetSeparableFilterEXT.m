function [ row, column, span ] = glGetSeparableFilterEXT( target, format, type )

% glGetSeparableFilterEXT  Interface to OpenGL function glGetSeparableFilterEXT
%
% usage:  [ row, column, span ] = glGetSeparableFilterEXT( target, format, type )
%
% C function:  void glGetSeparableFilterEXT(GLenum target, GLenum format, GLenum type, void* row, void* column, void* span)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

row = (0);
column = (0);
span = (0);

moglcore( 'glGetSeparableFilterEXT', target, format, type, row, column, span );

return
