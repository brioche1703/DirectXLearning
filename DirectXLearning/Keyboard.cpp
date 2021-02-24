#include "Keyboard.h"

bool Keyboard::KeyIsPressed(unsigned char keycode) const noexcept {
	return keyStates[keycode];
}

Keyboard::Event Keyboard::ReadKey() noexcept {
	if (keyBuffer.size() > 0u) {
		Keyboard::Event e = keyBuffer.front();
		keyBuffer.pop();
		return e;
	}
	else {
		return Keyboard::Event();
	}
}

bool Keyboard::KeyIsEmpty() const noexcept {
	return keyBuffer.empty();
}

void Keyboard::FlushKey() noexcept {
	keyBuffer = std::queue<Event>();
}

char Keyboard::ReadChar() noexcept {
	if (charBuffer.size() > 0u) {
		unsigned char charCode = charBuffer.front();
		charBuffer.pop();
		return charCode;
	}
	else {
		return 0;
	}
}

bool Keyboard::CharIsEmpty() const noexcept {
	return charBuffer.empty();
}

void Keyboard::FlushChar() noexcept {
	charBuffer = std::queue<char>();
}

void Keyboard::Flush() noexcept {
	FlushKey();
	FlushChar();
}

void Keyboard::EnableAutorepeat() noexcept {
	autorepeatEnabled = true;
}

void Keyboard::DisableAutorepeat() noexcept {
	autorepeatEnabled = false;
}

bool Keyboard::AutorepeatIsEnabled() noexcept {
	return autorepeatEnabled;
}

void Keyboard::OnKeyPressed(unsigned char keycode) noexcept {
	keyStates[keycode] = true;
	keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));
	TrimBuffer(keyBuffer);
}

void Keyboard::OnKeyReleased(unsigned char keycode) noexcept {
	keyStates[keycode] = false;
	keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
	TrimBuffer(keyBuffer);
}

void Keyboard::OnChar(char character) noexcept {
	charBuffer.push(character);
	TrimBuffer(charBuffer);
}

void Keyboard::ClearState() noexcept {
	keyStates.reset();
}

template<typename T>
static void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept {
	while (buffer.size() > bufferSize) {
		buffer.pop();
	}
}
