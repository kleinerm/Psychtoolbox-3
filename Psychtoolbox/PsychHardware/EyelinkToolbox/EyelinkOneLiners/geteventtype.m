function evtstr=geteventtype(el, type)

switch( type )
    % EVENT types
    case el.SAMPLE_TYPE,
        evtstr='sample';
    case el.STARTPARSE, % 	/* these only have time and eye data */
        evtstr='startparse';
    case el.ENDPARSE,
        evtstr='endparse';
    case el.BREAKPARSE,
        evtstr='breakparse';
    case el.STARTBLINK,    % /* EYE DATA: contents determined by evt_data */
        evtstr='startblink';
    case el.ENDBLINK,   	% /* and by "read" data item */
        evtstr='endblink';
    case el.STARTSACC,		% /* all use IEVENT format */
        evtstr='startsacc';
    case el.ENDSACC,
        evtstr='endsacc';
    case el.STARTFIX,
        evtstr='startfix';
    case el.ENDFIX,
        evtstr='endfix';
    case el.FIXUPDATE,
        evtstr='fixupdate';
    case el.STARTSAMPLES,  	%/* start of events in block *//* control events: all put data into */
        evtstr='startsamples';
    case el.ENDSAMPLES,  		%/* end of samples in block *//* the EDF_FILE or ILINKDATA status  */
        evtstr='endsamples';
    case el.STARTEVENTS, 		% /* start of events in block */
        evtstr='startevents';
    case el.ENDEVENTS,  		%/* end of events in block */
        evtstr='endevents';
    case el.MESSAGEEVENT,  % /* user-definable text or data */
        evtstr='messageevent';
    case el.BUTTONEVENT,  %/* button state change */
        evtstr='buttonevent';
    case el.INPUTEVENT,  % /* change of input port */
        evtstr='inputevent';
    case el.LOSTDATAEVENT,  %/* flags a gap in the data stream due to queue filling up */
        evtstr='lostdataevent';
    otherwise,
        evtstr='unknown';
end
