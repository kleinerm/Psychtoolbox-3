function DatapixxInfo()
% DatapixxInfo()
%
% Print summary of all Datapixx systems.
% If the Datapixx is closed, it will be opened then closed by this function.
% If the Datapixx is already open, it will be left open by this function.
%
% History:
%
% Oct 1, 2009  paa     Written

AssertOpenGL;   % We use PTB-3

% Before touching API, report any global errors
libraryError = Datapixx('GetError');
if (libraryError)
    fprintf('Datapixx library error code = %d\n', libraryError);
end

datapixxWasOpen = Datapixx('IsReady');
if (datapixxWasOpen)
    fprintf('\nDatapixx device is open\n');
else
    fprintf('\nDatapixx device is not open\n');
    Datapixx('Open');
end

%Take a register snapshot
Datapixx('RegWrRd');

% Print some general system info
fprintf('Datapixx has %dMB of RAM\n', Datapixx('GetRamSize')/2^20);
fprintf('Datapixx firmware revision = %d\n', Datapixx('GetFirmwareRev'));
fprintf('Datapixx time = %g seconds\n', Datapixx('GetTime'));
fprintf('Datapixx marker = %g seconds\n', Datapixx('GetMarker'));
fprintf('Datapixx power supply is providing %4.2fV at %4.2fA\n', Datapixx('GetSupplyVoltage'), Datapixx('GetSupplyCurrent'));
if (Datapixx('Is5VFault'))
    fprintf('Datapixx is detecting a short circuit on Analog I/O or VESA 3D +5V pins\n');
end
fprintf('Datapixx internal temperature is %dC = %dF\n', Datapixx('GetTempCelcius'), Datapixx('GetTempFarenheit'));

% Print each of the subsystem status structures
fprintf('\nDatapixx DAC status:\n'); disp(Datapixx('GetDacStatus'));
fprintf('\nDatapixx ADC status:\n'); disp(Datapixx('GetAdcStatus'));
fprintf('\nDatapixx Digital Output status:\n'); disp(Datapixx('GetDoutStatus'));
fprintf('\nDatapixx Digital Input status:\n'); disp(Datapixx('GetDinStatus'));
fprintf('\nDatapixx Audio Output status:\n'); disp(Datapixx('GetAudioStatus'));
fprintf('\nDatapixx Audio Input status:\n'); disp(Datapixx('GetMicrophoneStatus'));
fprintf('\nDatapixx Video status:\n'); disp(Datapixx('GetVideoStatus'));

% Wrapup
if (~datapixxWasOpen)
    Datapixx('Close');
end
fprintf('\nReport complete\n\n');
