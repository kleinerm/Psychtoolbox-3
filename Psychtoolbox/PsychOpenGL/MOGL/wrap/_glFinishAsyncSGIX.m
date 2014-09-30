function [ r, markerp ] = glFinishAsyncSGIX

% glFinishAsyncSGIX  Interface to OpenGL function glFinishAsyncSGIX
%
% usage:  [ r, markerp ] = glFinishAsyncSGIX
%
% C function:  GLint glFinishAsyncSGIX(GLuint* markerp)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=0,
    error('invalid number of arguments');
end

markerp = uint32(0);

r = moglcore( 'glFinishAsyncSGIX', markerp );

return
