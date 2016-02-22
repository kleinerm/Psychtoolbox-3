function psychwavwrite(varargin)
% psychwavwrite - Replacement for wavwrite().
%
% Replaces the Matlab wavwrite() function, which was removed
% in Matlab R2015b, by the audiowrite() function, which was
% introduced in R2012b, to provide basic sanity to the mess
% that is Matlab's way of (not) dealing with "backwards
% compatibility".
%
% This is a least common denominator implementation of
% what both wavwrite() and audiowrite() support in a
% compatible fashion. See the help of either one for
% details.
%
% Usage:
%
%  wavwrite(y,filename)
%  wavwrite(y,Fs,filename)
%  wavwrite(y,Fs,N,filename)

% History:
% 02-Feb-2016 mk  Created.

if exist('audioread')
  if length(varargin) < 2
    error('Invalid call to psychwavwrite(). Must provide at least y and filename.');
  end

  if length(varargin) == 2
    audiowrite(varargin{2}, varargin{1}, 8000);
  elseif length(varargin) == 3
    audiowrite(varargin{3}, varargin{1}, varargin{2});
  elseif length(varargin) == 4
    audiowrite(varargin{4}, varargin{1}, varargin{2}, 'BitsPerSample', varargin{3});
  else
    error('Too many input arguments to psychwavwrite().');
  end
else
  wavwrite(varargin{:});
end
