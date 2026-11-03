#include "Global.hpp"

Global::State Global::state = Global::State::Disconnected;
volatile bool Global::mic_enabled = false;
