function glDeleteObjectARB( obj )

% glDeleteObjectARB  Interface to OpenGL function glDeleteObjectARB
%
% usage:  glDeleteObjectARB( obj )
%
% C function:  void glDeleteObjectARB(GLhandleARB obj)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glDeleteObjectARB', obj );

return
