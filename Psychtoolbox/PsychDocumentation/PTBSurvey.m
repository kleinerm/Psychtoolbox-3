function PTBSurvey
%  We are currently conducting a user survey for Psychtoolbox.
%  Please participate, the sooner the better! We need one person
%  per lab to respond, ideally one with sufficient insight into
%  the operation of the lab, the labs needs, and the labs ability
%  to contribute funding to PTB. In other words, the PI of the lab,
%  or maybe a lab manager might be best suitable to fill out the survey.
%
%  The outcome of this survey may help to secure some short-term,
%  time-limited bridge funding for Psychtoolbox development,
%  maintenance and support. It will also influence decisions on how to
%  fund future development, as well as priorities in development wrt.
%  supporting new functionality or hardware.
%
%  The survey has a mandatory part of six questions, important for
%  funding decisions and PTB’s future, and an optional longer part of
%  optional questions, mostly to determine how you use PTB, what your
%  needs and wishes are, what hardware/software you use.
%
%  Please point your web browser to this URL in order to participate
%  in the survey:
%
%  http://goo.gl/forms/LlD2XbNf0XNYuq7B2
%
%  Thanks!

try
    fid = fopen([PsychtoolboxConfigDir 'surveydone'], 'w');
    fclose(fid);
catch
end

help PTBSurvey;
