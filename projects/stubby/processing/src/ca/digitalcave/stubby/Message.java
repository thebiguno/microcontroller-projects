package ca.digitalcave.stubby;

public class Message {
	private final int command;
	private final int[] data;
	
	public Message(int command, int[] data) {
		this.command = command;
		this.data = data;
	}
	
	public int getCommand() {
		return command;
	}
	
	public int[] getData() {
		return data;
	}
}
