function [ r, markerp ] = glPollAsyncSGIX

% glPollAsyncSGIX  Interface to OpenGL function glPollAsyncSGIX
%
% usage:  [ r, markerp ] = glPollAsyncSGIX
%
% C function:  GLint glPollAsyncSGIX(GLuint* markerp)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=0,
    error('invalid number of arguments');
end

markerp = uint32(0);

r = moglcore( 'glPollAsyncSGIX', markerp );

return
