#pragma once

#include <string>
#include <assert.h>
#include <ctime>

#define TRACEMESSAGE_BUFFER_COUNT 80

class TraceMessage {
public:
	TraceMessage();

	void SetUseWindowsCRLF(bool useWindowsCRLF) { this->useWindowsCRLF = useWindowsCRLF; }

	// Add a message in this object.
	void WriteMessage(const std::string &msg);
	// Add a message in this object, but does not mark as "not read"
	void WriteMessageNoNotification(const std::string &msg);
	// Get the last message that has been written
	std::string GetLastMessage();
	// Get a multi-line string of all (last TRACEMESSAGE_BUFFER_COUNT) messages.
	std::string GetAllMessages(bool reverseOrder = false);
	// Mark all messages as read.
	void MarkAsRead(bool isRead) {
		this->hasUnreadMessages = !isRead;
	}
	bool HasUnreadMessages() {
		return this->hasUnreadMessages;
	}

private:
	std::string allMessages[TRACEMESSAGE_BUFFER_COUNT];
	time_t allMessagesTime[TRACEMESSAGE_BUFFER_COUNT];
	int currentIndex; // index of last written message.
	int messagesCount; // how many messages in array are relevant ? Useful at beginning when less than TRACEMESSAGE_BUFFER_COUNT have been written.
	bool useWindowsCRLF;
	bool hasUnreadMessages;

	// Securely get a message time provided its "raw" index.
	const time_t *GetMessageTime(int rawIndex);
	// Securely get a message provided its "raw" index.
	std::string GetMessage(int rawIndex);
	// Securely set a message in array, provided its "raw" index.
	void SetMessage(int rawIndex, const std::string &msg);
};


// Define it only once (in .cpp file) so all files will use the same instance.
extern TraceMessage traceMessageHandler;
