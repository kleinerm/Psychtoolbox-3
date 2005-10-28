% Psychtoolbox:Quest.
%
% This Quest toolbox is distributed both on its own and as part of the Psychtoolbox.
% web http://psych.nyu.edu/pelli/software.html#quest
% web http://psychtoolbox.org/
% 
% INTRODUCTORY FUNCTION THAT WILL HELP YOU GET STARTED
%  QuestDemo            - Demo script for Quest routines. Explanations and advice.
% HIGH LEVEL FUNCTIONS THAT YOU'LL USE EVERY DAY
%  QuestCreate          - Initialize Quest parameters.
%  QuestUpdate          - Update Quest posterior pdf.
%  QuestMean            - Mean of Quest posterior pdf. Recommended by Pelli(1987) and King-Smith et al.(1994).
%  QuestMode            - Mode of Quest posterior pdf. Recommended by Watson & Pelli(1983).
%  QuestQuantile        - Get Quest recommendation for next trial intensity. Recommended by Pelli(1987).
%  QuestSd              - Standard deviation of Quest posterior pdf.
% LOW LEVEL FUNCTIONS FOR SPECIAL PURPOSES
%  QuestBetaAnalysis    - Estimate threshold and beta.
%  QuestP               - Probability of a correct response, i.e. the psychometric function.
%  QuestPdf             - pdf of threshold. (May need to be normalized.)
%  QuestRecompute       - Recompute the psychometric function & pdf.
%  QuestSimulate        - Simulate an observer with given Quest parameters.
%  QuestTrials          - Sorted list of intensities and response frequencies.

% Copyright (c) 1996-2005 by Denis Pelli
