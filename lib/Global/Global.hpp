#pragma once

namespace Global {
  enum class State {
    Connecting,
    Connected,
    Disconnected
  };

  extern State state;
  extern volatile bool mic_enabled;
}
