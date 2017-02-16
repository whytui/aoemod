#include "../include/traceMessage.h"

TraceMessage traceMessageHandler;

TraceMessage::TraceMessage() {
	this->useWindowsCRLF = true;
	this->hasUnreadMessages = false;
	this->currentIndex = 0;
	this->messagesCount = 0;
	for (int i = 0; i < TRACEMESSAGE_BUFFER_COUNT; i++) {
		this->SetMessage(i, "");
	}
}


// Add a message in this object.
void TraceMessage::WriteMessage(const std::string &msg) {
	this->hasUnreadMessages = true;
	this->WriteMessageNoNotification(msg);
}

// Add a message in this object, but does not mark as "not read"
void TraceMessage::WriteMessageNoNotification(const std::string &msg) {
	this->currentIndex++;
	if (this->currentIndex >= TRACEMESSAGE_BUFFER_COUNT) {
		this->currentIndex = 0;
	}
	if (this->messagesCount < TRACEMESSAGE_BUFFER_COUNT) {
		this->messagesCount++;
	}
	this->SetMessage(this->currentIndex, msg);
}


// Get the last message that has been written
std::string TraceMessage::GetLastMessage() {
	this->hasUnreadMessages = false;
	if (this->messagesCount <= 0) {
		return "";
	}
	return this->GetMessage(this->currentIndex);
}


// Get a multi-line string of all (last TRACEMESSAGE_BUFFER_COUNT) messages.
std::string TraceMessage::GetAllMessages(bool reverseOrder) {
	this->hasUnreadMessages = false;
	if (this->messagesCount <= 0) {
		return "";
	}
	int index = this->currentIndex; // if reverse, start from last-written entry
	if (!reverseOrder) {
		// Otherwise, start from oldest (valid) entry
		index = this->currentIndex + 1 - this->messagesCount;
		index = index % TRACEMESSAGE_BUFFER_COUNT; // warning: modulo for a negative value has a negative result
		if (index < 0) { index += TRACEMESSAGE_BUFFER_COUNT; } // Fix negative values
	}
	std::string result = "";
	for (int count = 0; count < this->messagesCount; count++) {
		tm ltm;
		errno_t e = localtime_s(&ltm, this->GetMessageTime(index));
		if (e) {} else {
			char buf[12];
			sprintf_s(buf, "[%02d:%02d:%02d] ", ltm.tm_hour, ltm.tm_min, ltm.tm_sec);
			result += buf;
		}
		result += this->GetMessage(index);
		result += "\r\n";
		if (reverseOrder) {
			index--;
			if (index < 0) {
				index = TRACEMESSAGE_BUFFER_COUNT - 1;
			}
		} else {
			index++;
			if (index >= TRACEMESSAGE_BUFFER_COUNT) {
				index = 0;
			}
		}
	}
	return result;
}


// Securely get a message time provided its "raw" index.
const time_t *TraceMessage::GetMessageTime(int rawIndex) {
	assert((rawIndex >= 0) && (rawIndex < TRACEMESSAGE_BUFFER_COUNT));
	int i = rawIndex % TRACEMESSAGE_BUFFER_COUNT;
	return &this->allMessagesTime[i];
}


// Securely get a message provided its "raw" index.
std::string TraceMessage::GetMessage(int rawIndex) {
	assert((rawIndex >= 0) && (rawIndex < TRACEMESSAGE_BUFFER_COUNT));
	int i = rawIndex % TRACEMESSAGE_BUFFER_COUNT;
	return this->allMessages[i];
}


// Securely set a message in array, provided its "raw" index.
void TraceMessage::SetMessage(int rawIndex, const std::string &msg) {
	assert((rawIndex >= 0) && (rawIndex < TRACEMESSAGE_BUFFER_COUNT));
	int i = rawIndex % TRACEMESSAGE_BUFFER_COUNT;
	this->allMessages[i] = msg;
	this->allMessagesTime[i] = time(NULL);
}
