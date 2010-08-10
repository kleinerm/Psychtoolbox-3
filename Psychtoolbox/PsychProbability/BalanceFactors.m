function varargout = BalanceFactors (N, randomize, varargin)

% BalanceFactors balances a set of factors given the factor levels.  It is
% identical to BalanceTrials except that the first argument in this
% function is the number of replicates per cell.  It outputs one or more
% vectors or cell arrays, each containing factor values for a set of
% trials, balanced and, optionally, randomized.
%
% [F1, F2, ...] = BalanceFactors(N, RND, LVL1, LVL2, ...)
%
% BalanceFactors must be called with three or more input arguments.  The
% first argument, N, specifies the number of replicates per combination of
% factor levels.  The second argument, RND, determines whether or not the
% returned factors should be shuffled (non-zero values lead to shuffling).
% 
% The remaining input arguments specify the levels for each of a set of
% factors.  Factor levels can be specified as numeric vectors or cell
% arrays (e.g., for category names).  The returned factor lists will be the
% same class as the corresponding levels.
%
% EXAMPLES:
%
%  [targetPresent, setSize] = BalanceFactors(2, 0, 0:1, [3 6 9 12]);
%
%  [target, setSize, dur] = ...
%     BalanceFactors(1, 1, [0 1], [4 8 12], [0 100 200]);
%
%  [samediff, mask] = ...
%     BalanceFactors(3, 1, {'same', 'diff'}, {'none', 'pattern', 'meta'});
%
% See also: BalanceTrials

% Author: David E. Fencsik (david.fencsik@csueastbay.edu)
% $LastChangedDate: 2007-06-06 10:56:41 -0400 (Wed, 06 Jun 2007) $

%%% BEGIN ARGUMENT CHECKING %%%
if nargin < 3
   error('%s must have at least three input arguments', mfilename);
end
if numel(N) ~= 1
   error('First argument to %s must be a single integer', mfilename);
end
if numel(randomize) ~= 1
   error('Second argument to %s must be a single integer', mfilename);
end
% make sure number of input IVs match number of output IVs
nFactors = nargin - 2;
if nargout ~= nFactors
   error('%d input argument(s) does not match %d output arguments', ...
         nFactors, nargout);
end
%%% END ARGUMENT CHECKING %%%

% count up number of levels in each factor and the minimum number of
% trials needed for one observation per cell
nLevels = zeros(nFactors, 1);
for f = 1:nFactors
   nLevels(f) = length(varargin{f});
end
minTrials = prod(nLevels);

% initialize cell array that will hold balanced variables
varargout = cell(1, nFactors);

% the following initializes and runs the main loop in the function,
% which generates enough repetitions of each factor, ensuring a
% balanced design, and randomizes them
len1 = minTrials;
len2 = 1;
[dummy, index] = sort(rand(N * minTrials, 1));
for f = 1:nFactors
   len1 = len1 / nLevels(f);
   if size(varargin{f}, 1) ~= 1
      % ensure that factor levels are arranged in one row
      varargin{f} = reshape(varargin{f}, 1, numel(varargin{f}));
   end
   % this is the critical line: it ensures there are enough repetitions
   % of the current factor in the correct order
   varargout{f} = repmat(reshape(repmat(varargin{f}, len1, len2), minTrials, 1), N, 1);
   if randomize
      varargout{f} = varargout{f}(index);
   end
   len2 = len2 * nLevels(f);
end
