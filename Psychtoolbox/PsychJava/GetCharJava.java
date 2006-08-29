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
	private int charCount = 0;
	private final int bufferSize = 1000;
	private KeyboardFocusManager manager;
	private boolean registered = false;

	public GetCharJava()
	{
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
		char k;

		if (e.getID() == KeyEvent.KEY_PRESSED) {
			k = e.getKeyChar();
			if (charCount < bufferSize) {	
				charBuffer[charCount] = k;
				eventModifiersBuffer[charCount] = e.getModifiersEx();
				eventTimesBuffer[charCount] = e.getWhen();
			}

			charCount++;
		}

		return false;
	}

	
	public void clear()
	{
		charCount = 0;
	}


	public int getCharCount()
	{
		// If we've exceeded the buffer size, return -1.
		if (charCount >= bufferSize) {
			return -1;
		}
		else {
			return charCount;
		}
	}

	public int getChar()
	{
		int k;

		if (charCount == 0) {
			k = 0;
		}
		else if (charCount >= bufferSize) {
			k = -1;
		}
		else {
			// Grab the most recent character, modifier, and time from
			// the buffers.
			charCount--;
			currentChar = charBuffer[charCount];
			currentModifier = eventModifiersBuffer[charCount];
			currentTime = eventTimesBuffer[charCount];
			
			k = (int)currentChar;
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
		manager.addKeyEventDispatcher(this);
		registered = true;
	}


	public void unregister()
	{
		manager.removeKeyEventDispatcher(this);
		registered = false;
	}
}
