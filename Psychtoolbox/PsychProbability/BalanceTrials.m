function varargout = BalanceTrials (nTrials, randomize, varargin)

% BalanceTrials balances a set of factors given the factor levels.  It is
% identical to BalanceFactors except that the first argument is the number
% of trials desired.  It outputs one or more vectors containing factor
% values for each trial, balanced and, optionally, randomized.
%
% [F1, F2, ...] = BalanceTrials(NTRIALS, RND, LVL1, LVL2, ...)
%
% BalanceTrials must be called with three or more input arguments.  The
% first argument, NTRIALS, specifies the number of trials desired.  The
% second argument, RAND, determines whether or not the returned factors
% should be shuffled (non-zero values lead to shuffling).
% 
% The remaining input arguments specify the levels for each of a set of
% factors.  Factor levels can be specified as numeric vectors or cell
% arrays (e.g., for category names).  The returned factor lists will be the
% same class as the corresponding levels.
%
% WARNING: If NTRIALS is not a multiple of the product of the number of
% levels, then the actual number of trials generated will be more than
% NTRIALS.  To detect this situation, test whether numel(F1) == NTRIALS.
%
% EXAMPLES:
%
%  [targetPresent, setSize] = BalanceTrials(80, 0, 0:1, [3 6 9 12]);
%
%  [target, setSize, dur] = ...
%     BalanceTrials(72, 1, [0 1], [4 8 12], [0 100 200]);
%
%  [samediff, mask] = ...
%     BalanceTrials(20, 1, {'same', 'diff'}, {'pattern', 'meta'});
%
% See also: BalanceFactors

% Author: David E. Fencsik (david.fencsik@csueastbay.edu)
% Last Changed: July 6, 2010

%%% BEGIN ARGUMENT CHECKING %%%
if nargin < 3
   error('%s must have at least three input arguments', mfilename);
end
if numel(nTrials) ~= 1
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

% count up number of levels in each factor and the minimum number of trials
% needed for one observation per cell
nLevels = zeros(nFactors, 1);
for f = 1:nFactors
   nLevels(f) = length(varargin{f});
end
minTrials = prod(nLevels);

% determine the number of replicates
N = ceil(nTrials / minTrials);

% initialize cell array that will hold balanced variables
varargout = cell(1, nFactors);

% the following initializes and runs the main loop in the function, which
% generates enough repetitions of each factor, ensuring a balanced design,
% and randomizes them
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
   varargout{f} = repmat(reshape(repmat(varargin{f}, len1, len2), ...
                                 minTrials, 1), N, 1);
   if randomize
      varargout{f} = varargout{f}(index);
   end
   len2 = len2 * nLevels(f);
end
