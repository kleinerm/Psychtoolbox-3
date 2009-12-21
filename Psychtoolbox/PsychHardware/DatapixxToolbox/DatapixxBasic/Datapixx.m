function varargout = Datapixx(varargin)
% Datapixx is a MEX file for precise control of the DataPixx device from
% VPixx Technologies. It has many functions; type "Datapixx" for a list:
% 	Datapixx
%
% For explanation of any particular Datapixx function, just add a question
% mark "?". E.g. for 'Open', try either of these equivalent forms:
% 	Datapixx('Open?')
% 	Datapixx Open?
%
% Most of the time you won't use this function directly, but instead use
% the PsychDataPixx() function for more convenient control and execution of
% common tasks.
%
% For setup of Datapixx video operations, check the online help of
% PsychImaging(), which has multiple functions for interacting with the
% Datapixx device.
%
% For an overview of demos and other useful helper functions for the DataPixx,
% type "help DatapixxToolbox".
%
%

AssertMex('Datapixx.m');
