function [ length, source ] = glGetShaderSourceARB( obj, maxLength )

% glGetShaderSourceARB  Interface to OpenGL function glGetShaderSourceARB
%
% usage:  [ length, source ] = glGetShaderSourceARB( obj, maxLength )
%
% C function:  void glGetShaderSourceARB(GLhandleARB obj, GLsizei maxLength, GLsizei* length, GLcharARB* source)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

length = int32(0);
source = uint8(0);

moglcore( 'glGetShaderSourceARB', obj, maxLength, length, source );

return
