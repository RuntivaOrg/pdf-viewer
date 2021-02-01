#include "events/text_input_event.h"

TextInputEvent::TextInputEvent(const std::wstring &text):
  text_{text}
{}

std::wstring TextInputEvent::text() const
{
  return text_;
}
