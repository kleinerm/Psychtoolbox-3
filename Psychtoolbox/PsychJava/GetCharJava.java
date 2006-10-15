import java.awt.KeyEventDispatcher;
import java.awt.event.KeyEvent;
import java.awt.event.InputEvent;
import java.awt.KeyboardFocusManager;

public class GetCharJava implements KeyEventDispatcher
{
	private char[] charBuffer;
	private char currentChar;
	private int[] eventModifiersBuffer;
	private int currentModifier;
	private long[] eventTimesBuffer;
	private long currentTime;
	private int queueHead;          // Index of the queue head.
    	private int writeHead;          // Index of the write head.
	private final int bufferSize = 1024;
	private KeyboardFocusManager manager;
	private boolean registered = false;
    	private boolean redispatchFlag = false;

	public GetCharJava()
	{
	    // Initialize all of our buffers and grab the keyboard focus manager.
	    manager = KeyboardFocusManager.getCurrentKeyboardFocusManager();
	    charBuffer = new char[bufferSize];
	    eventModifiersBuffer = new int[bufferSize];
	    eventTimesBuffer = new long[bufferSize];
	}

	protected void finalize()
	{
	    if (registered) {
		this.unregister();
	    }
	}

	public boolean dispatchKeyEvent(KeyEvent e)
	{
	    if (e.getID() == KeyEvent.KEY_TYPED) {
		int bufIndex = writeHead % bufferSize;
            
		// Grab the character and stick it and its associated
		// information into the queue.
		charBuffer[bufIndex] =  e.getKeyChar();
		eventModifiersBuffer[bufIndex] = e.getModifiersEx();
		eventTimesBuffer[bufIndex] = e.getWhen();
		
		writeHead++;

		// Check if the CTRL+C combo was pressed: If so, we (re-)enable keyboard
		// event dispatching (aka redispatchFlag=false). This way, the CTRL+C
		// abort request will reach Matlab and keyboard input into Matlab gets
		// reenabled.
		if ((((int) e.getKeyChar())==3) || (e.getKeyChar()=='c') && ((e.getModifiersEx() & InputEvent.CTRL_DOWN_MASK) > 0)) {
		    // CTRL+C pressed! Enable redispatching:
		    redispatchFlag = false;
		}
	    }


	    // By returning a value of 'false', we let other event dispatchers handle
	    // what we've caught. This will cause Matlab to receive the character as
	    // well. By returning 'true', we do not redispatch. Matlab does not receive
	    // the character. This prevents clutter in the command window and other
	    // windows, but in case of script abortion due to error, it may also leave
	    // Matlab with a dead keyboard. The redispatchFlag defaults to 'false', aka
	    // redispatch. It can be set via call to setRedispatchFlag().
	    return(redispatchFlag);
	}
    
	
        public void clear()
        {
	    queueHead = 0;
	    writeHead = 0;
	}


	public int getCharCount()
	{
	    int charCount = writeHead - queueHead;
	    
	    // If we've exceeded the buffer size, return -1.
	    if (charCount > bufferSize) {
		return -1;
	    }
	    else {
		return charCount;
	    }
	}

	public int getChar()
	{
	    int charCount = writeHead - queueHead, k;
	    
	    if (charCount == 0) {
		k = 0;
	    }
	    else if (charCount > bufferSize) {
		k = -1;
	    }
	    else {
		// Grab the most recent character, modifier, and time from
		// the buffers.
		int bufIndex = queueHead % bufferSize;
		currentChar = charBuffer[bufIndex];
		currentModifier = eventModifiersBuffer[bufIndex];
		currentTime = eventTimesBuffer[bufIndex];
		
		k = (int)currentChar;
		
		queueHead++;
	    }
	    
	    return k;
	}
	
	
	public boolean[] getModifiers()
	{
	    boolean[] modVals = new boolean[4];
	    
	    // command
	    modVals[0] = (InputEvent.META_DOWN_MASK & currentModifier) == InputEvent.META_DOWN_MASK;
	    
	    // control
	    modVals[1] = (InputEvent.CTRL_DOWN_MASK & currentModifier) == InputEvent.CTRL_DOWN_MASK;
	    
	    // option/alt
	    modVals[2] = (InputEvent.ALT_DOWN_MASK & currentModifier) == InputEvent.ALT_DOWN_MASK;
	    
	    // shift
	    modVals[3] = (InputEvent.SHIFT_DOWN_MASK & currentModifier) == InputEvent.SHIFT_DOWN_MASK;
	    
	    return modVals;
	}
    
        public long getEventTime()
	{
	    return currentTime;
	}


	public void register()
	{
	    // Only add the dispatcher if we're not registered.
	    if (!registered) {
		// Initilize our indexes to zero so we start with
		// a fresh ring buffer.
		queueHead = 0;
		writeHead = 0;
		
		manager.addKeyEventDispatcher(this);
		registered = true;
	    }
	}


	public void unregister()
	{
	    // Only remove the dispatcher if we're registered.
	    if (registered) {
    		manager.removeKeyEventDispatcher(this);
        	registered = false;
	    }
	}

        public void setRedispatchFlag(int newflag)
        {
	    redispatchFlag = (newflag > 0) ? true : false;
    	}
}
