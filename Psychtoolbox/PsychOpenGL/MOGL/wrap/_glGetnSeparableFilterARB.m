function [ row, column, span ] = glGetnSeparableFilterARB( target, format, type, rowBufSize, columnBufSize )

% glGetnSeparableFilterARB  Interface to OpenGL function glGetnSeparableFilterARB
%
% usage:  [ row, column, span ] = glGetnSeparableFilterARB( target, format, type, rowBufSize, columnBufSize )
%
% C function:  void glGetnSeparableFilterARB(GLenum target, GLenum format, GLenum type, GLsizei rowBufSize, GLvoid* row, GLsizei columnBufSize, GLvoid* column, GLvoid* span)

% 30-Aug-2012 -- created (generated automatically from header files)

% ---allocate---

if nargin~=5,
    error('invalid number of arguments');
end

row = (0);
column = (0);
span = (0);

moglcore( 'glGetnSeparableFilterARB', target, format, type, rowBufSize, row, columnBufSize, column, span );

return
