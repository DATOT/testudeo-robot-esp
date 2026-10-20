namespace Global {
  enum class State {
    Connecting,
    Connected,
    Disconnected
  };
  extern State state;
};