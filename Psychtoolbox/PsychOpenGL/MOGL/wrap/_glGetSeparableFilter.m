function [ row, column, span ] = glGetSeparableFilter( target, format, type )

% glGetSeparableFilter  Interface to OpenGL function glGetSeparableFilter
%
% usage:  [ row, column, span ] = glGetSeparableFilter( target, format, type )
%
% C function:  void glGetSeparableFilter(GLenum target, GLenum format, GLenum type, GLvoid* row, GLvoid* column, GLvoid* span)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

row = (0);
column = (0);
span = (0);

moglcore( 'glGetSeparableFilter', target, format, type, row, column, span );

return
