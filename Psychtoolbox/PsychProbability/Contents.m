%   Psychtoolbox:PsychProbability.
%  
%   Computation/Evaluation of probability distributions, basic sampling and
%   randomization.
%
%   help Psychtoolbox % For an overview, triple-click me & hit enter.
%  
%   BalanceFactors      - Balance a set of factors given the factor levels.
%   BalanceTrials       - Balance a set of factors given the factor levels. Takes alternative parameters wrt. BalanceFactors.
%   BuildMarkovK        - Build covariance matrix for Markov process.
%   ChiSqrCumulative    - Chi-squared distribution probability function.
%   ChiSquarePDF        - Chi-squared distribution PDF.
%   ChooseKFromN        - Randomly choose k distinct integers out of n.  
%   ClockRandSeed       - Seed rand() and randn() from clock.
%   CoinFlip            - Bernoulli random variable.
%   CoinFlipPDF         - Bernoulli distribution PDF.
%   ColMean             - Take column means, works for one row matrices.
%   ColStd              - Take column stds, works for one row matrices.
%   CombVec             - Generate all possible combinations of input vectors.
%   CovToCorr           - Compute matrix of correlations from covariance.
%   FindNeighborCorr    - Find nearest neighbor correlations in data set.
%   MultiNormalDraw     - Draw vectors from multivariate normal.
%   MultiNormalPDF      - Compute multivariate normal PDF.
%   NormalCumulative    - Univariate normal CDF
%   NormalDraw          - Draw from univariate normal.
%   NormalPDF           - Univariate normal PDF.
%   NormalProb          - Compute probability of univariate normal in interval.
%   NRandPerm           - Randomly select elements from a random permutation.
%   RandDim             - Randomize matrix along any dimension.
%   Randi               - Get random integer sample. Same as Ranint, but Denis prefers it.
%   RandLim             - Generates a matrix of random numbers between a lower and upper limit.
%   RandSample          - Get a random sample from a list.
%   RandSel             - Randomly select elements from matrix, elements are replenished.
%   Ranint              - Get random integer sample. Same as Randi, but David prefers it.
%   Sample              - Get a random sample from a list.
%   Shuffle             - Randomly reorder the entries of vector/matrix.
%   UniqueAndN          - Returns unique elements of array and the number of times each element occurs in the array.
%   UniqueNoSorting     - Returns elements of array in order of appearance or disappearance.
%   URandSel            - Randomly select elements from matrix, elements are not replenished.
%
%
%   Combining CombVec and RandDim
%   Using CombVec and RandDim, it is very simple to generate all trials
%   in an experiment with a factorial design and then randomize these
%   trials:
%     cond_a = [1 2]; cond_b = [120 180]; ntrial = 2;
%     trial  = CombVec(cond_a,cond_b);
%     trial  =
%          1     2     1     2
%        120   120   180   180
%
%     trial           = repmat(trial,1,ntrial);   % each combination of conditions occurs ntrial times
%     trial(end+1,:)  = [1:size(trial,2)];        % append trial number
%     trial           = RandDim(trial,2);         % shuffle trial order
%
%     trial =
%          2     1     1     2     1     1     2     2
%        120   180   180   180   120   120   120   180
%          6     3     7     8     5     1     2     4
%
%
%   BalanceFactors.m was contributed by David E. Fencsik (fencsik@gmail.com), thanks!
