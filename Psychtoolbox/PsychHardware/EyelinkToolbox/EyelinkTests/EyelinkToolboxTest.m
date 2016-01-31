clear;
commandwindow;
fprintf('Testing EyelinkToolbox functions\n');
Eyelink
Eyelink('Version')
Eyelink('Shutdown')
fprintf('Opening EyelinkToolbox in dummy mode\n');
status = Eyelink('InitializeDummy')
if 1
Eyelink('Shutdown')

fprintf('Now opening EyelinkToolbox in REAL mode\n');

status = Eyelink('Initialize')
Eyelink('Shutdown')
fprintf('Opening EyelinkToolbox using EyelinkInit function\n');

EyelinkInit
Eyelink('Shutdown')
EyelinkInit
end
connect_status = Eyelink('IsConnected')

[version , versionString]  = Eyelink('GetTrackerVersion')
el=EyelinkInitDefaults()
Eyelink('OpenFile', 'demo.edf')
[key, el] =EyelinkGetKey(el)

command_status = Eyelink('Command', 'link_sample_data = LEFT,RIGHT,GAZE,AREA')
heuristic_filter_status = Eyelink('command', 'heuristic_filter = ON')
result = Eyelink('StartSetup')

% result = Eyelink('ImageModeDisplay')

status = Eyelink('DriftCorrStart', 100, 100)

result = Eyelink('ApplyDriftCorr')

[result, tx, ty] = Eyelink('TargetCheck')

AcceptTrigger_result = Eyelink('AcceptTrigger')

Eyelink('SetOfflineMode')

startrecording_error = Eyelink('StartRecording')

Message_status = Eyelink('Message', 'SYNCTIME')

RequestTime_status = Eyelink('RequestTime')

time = Eyelink('ReadTime')

WaitForModeReady_result = Eyelink('WaitForModeReady', 500)
CurrentMode_mode = Eyelink('CurrentMode')
CalResult_result = Eyelink('CalResult')


eyeused = Eyelink('EyeAvailable')

ButtonStates_result = Eyelink('ButtonStates')
% [result =] Eyelink('SendKeyButton', code, mods, state)

NewFloatSampleAvailable_status=Eyelink('NewFloatSampleAvailable')

connect_status = Eyelink('IsConnected')

sample = Eyelink('NewestFloatSample')

[sample, raw] = Eyelink('NewestFloatSampleRaw')

% Eyelink('Shutdown')
% return



type = Eyelink('GetNextDataType')

item = Eyelink('GetFloatData', type)


type = Eyelink('GetNextDataType')

[item raw]= Eyelink('GetFloatDataRaw', type)

[samples,events,drained] = Eyelink('GetQueuedData')

Eyelink('Stoprecording')


error = Eyelink('CheckRecording')


status = Eyelink('CloseFile')


Eyelink('Shutdown')

% [status =] Eyelink('ReceiveFile',['filename'], ['dest'], ['dest_is_path'])

return





Eyelink('Shutdown')
