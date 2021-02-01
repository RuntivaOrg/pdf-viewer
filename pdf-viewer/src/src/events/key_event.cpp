#include "events/key_event.h"

KeyEvent::KeyEvent(Key key, unsigned modifiers, bool autoRepeat):
  autoRepeat_(autoRepeat),
  key_(key),
  modifiers_(modifiers)
{
}

bool KeyEvent::isAutoRepeat() const
{
  return autoRepeat_;
}

KeyEvent::Key KeyEvent::key() const
{
  return key_;
}

unsigned KeyEvent::modifiers() const
{
  return modifiers_;
}
