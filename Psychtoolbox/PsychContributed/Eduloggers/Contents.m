% Psychtoolbox:PsychContributed:Eduloggers
%
% A folder with some simple but potentially useful Edulogger functions, for
% using Edulogger devices from Neulog: https://neulog.com
% contributed by Todd Parsons ( https://github.com/TEParsons ) under MIT license.
% Some modifications were made by Mario Kleiner to put them into an acceptable
% format for Psychtoolbox contributions.
%
% These are "as is" with no expectation of completeness, and NO TESTING was
% performed by Mario or Todd on the versions contained here, because Todd did
% not respond in time for the release, and Mario does not have the required
% Edulogger equipment for proper testing. That's why these files are in the
% PsychContributed folder, instead of the PsychHardware folder where properly
% tested and maintained code would go. USE AT YOUR OWN RISK, THIS IS UNSUPPORTED
% CODE!
%
% The original code can be found in this pull request:
% https://github.com/Psychtoolbox-3/Psychtoolbox-3/pull/526
%
% The Edulogger API docs can be found under:
% https://neulog.com/wp-content/uploads/2014/06/NeuLog-API-version-7.pdf
%
% This code may work on Linux, Windows and macOS with the Edulogger API server
% running on the local host on which Psychtoolbox/Octave/Matlab is running.
%
% It requires the webread() function, which is supported by Matlab R2014b or
% later, and by GNU/Octave 6.0 or later.
%
%
% EdulogRun         Run Eduloggers and return captured data.
% EdulogPlot        Plot data returned by EdulogRun().
% EdulogTest        A simple correctness test for EdulogRun and EdulogPlot.
% EdulogClapTest    A more complex test for EdulogRun and EdulogPlot.
%
