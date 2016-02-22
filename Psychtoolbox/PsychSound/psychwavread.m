function [y, Fs] = psychwavread(varargin)
% psychwavread - Replacement for wavread().
%
% Replaces the Matlab wavread() function, which was removed
% in Matlab R2015b, by the audioread() function, which was
% introduced in R2012b, to provide basic sanity to the mess
% that is Matlab's way of (not) dealing with "backwards
% compatibility".
%
% This is a least common denominator implementation of
% what both wavread() and audioread() support in a
% compatible fashion. See the help of either one for
% details.
%
% Usage:
%
% [y, Fs] = psychwavread(filename [, samples][, dataType]);
%

% History:
% 02-Feb-2016 mk  Created.

if exist('audioread')
  [y, Fs] = audioread(varargin{:});
else
  [y, Fs] = wavread(varargin{:});
end
