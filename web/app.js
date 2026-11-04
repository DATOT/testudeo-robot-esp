(() => {
  const WS_URL = 'ws://testudeo.local:8386/ws/stt';
  const FOCUS_SECONDS = 20 * 60;
  const BREAK_SECONDS = 5 * 60;
  const CIRCUMFERENCE = 2 * Math.PI * 143;

  const $ = (selector) => document.querySelector(selector);
  const socketLabel = $('#connection-label');
  const connectionPill = $('#connection-pill');
  const connectButton = $('#connect-button');
  const micButton = $('#mic-button');
  const listenCard = $('#listen-card');
  const toast = $('#toast');
  let socket = null;
  let listening = false;
  let toastTimeout = null;
  let timerInterval = null;
  let timerRunning = false;
  let timerPhase = 'focus';
  let remaining = FOCUS_SECONDS;

  function showToast(message) {
    toast.textContent = message;
    toast.classList.add('show');
    clearTimeout(toastTimeout);
    toastTimeout = setTimeout(() => toast.classList.remove('show'), 3400);
  }

  function connected() {
    return socket && socket.readyState === WebSocket.OPEN;
  }

  function setConnection(state) {
    connectionPill.classList.remove('connected', 'connecting');
    if (state === 'connected') {
      connectionPill.classList.add('connected');
      socketLabel.textContent = 'Robot connected';
      connectButton.innerHTML = 'Disconnect <span aria-hidden="true">×</span>';
      $('#chat-status').textContent = 'online';
      $('#chat-status').parentElement.classList.add('online');
      $('.robot-name-row .presence-dot').style.background = '#86c84b';
      $('#robot-status').textContent = 'Connected and ready';
    } else if (state === 'connecting') {
      connectionPill.classList.add('connecting');
      socketLabel.textContent = 'Connecting…';
      connectButton.textContent = 'Connecting…';
      $('#chat-status').textContent = 'connecting';
      $('#robot-status').textContent = 'Connecting to your robot';
    } else {
      socketLabel.textContent = 'Not connected';
      connectButton.innerHTML = 'Connect robot <span aria-hidden="true">↗</span>';
      $('#chat-status').textContent = 'offline';
      $('#chat-status').parentElement.classList.remove('online');
      $('.robot-name-row .presence-dot').style.background = '#a6d77a';
      $('#robot-status').textContent = 'Ready when you are';
    }
  }

  function send(payload) {
    if (!connected()) {
      showToast('Connect to the robot before sending a message.');
      return false;
    }
    socket.send(JSON.stringify(payload));
    return true;
  }

  function setListening(enabled, fromServer = false) {
    if (enabled && !$('#presentation-view').classList.contains('active')) {
      if (!fromServer) send({ type: 'mic', enabled: false });
      listening = false;
      renderListening();
      return;
    }
    listening = Boolean(enabled);
    renderListening();
  }

  function renderListening() {
    micButton.setAttribute('aria-pressed', String(listening));
    micButton.setAttribute('aria-label', listening ? 'Stop robot microphone listening' : 'Start robot microphone listening');
    micButton.title = listening ? 'Turn off the robot microphone' : 'Enable the robot microphone';
    $('#listen-title').textContent = listening ? 'Your robot is listening' : 'Microphone is off';
    $('#listen-description').textContent = listening ? 'Speak naturally. Tap the mic when you are done.' : 'Turn on listening to let your robot hear you.';
    listenCard.classList.toggle('listening', listening);
    micButton.classList.toggle('listening', listening);
  }

  function addMessage(text, sender = 'robot') {
    const messages = $('#messages');
    const hint = $('#empty-hint');
    if (hint) hint.remove();
    const row = document.createElement('div');
    row.className = `message-row ${sender === 'user' ? 'user-message' : 'robot-message'}`;
    const avatar = document.createElement('div');
    avatar.className = 'message-avatar';
    avatar.textContent = sender === 'user' ? 'Y' : 'T';
    const content = document.createElement('div');
    content.className = 'message-content';
    const author = document.createElement('span');
    author.className = 'message-author';
    author.textContent = sender === 'user' ? 'YOU ' : 'TESTUDEO ';
    const time = document.createElement('time');
    time.textContent = new Date().toLocaleTimeString([], { hour: 'numeric', minute: '2-digit' });
    author.append(time);
    const bubble = document.createElement('div');
    bubble.className = 'message-bubble';
    bubble.textContent = text;
    content.append(author, bubble);
    row.append(avatar, content);
    messages.append(row);
    messages.scrollTop = messages.scrollHeight;
  }

  function handleMessage(event) {
    if (typeof event.data !== 'string') return;
    let message;
    try { message = JSON.parse(event.data); } catch { addMessage(event.data); return; }
    switch (message.type) {
      case 'mic':
        setListening(Boolean(message.enabled), true);
        break;
      case 'chat':
      case 'message':
      case 'transcript':
        if (typeof (message.message || message.text) === 'string') addMessage(message.message || message.text);
        break;
      case 'error':
        showToast(message.message || 'The robot reported an error.');
        break;
      case 'done':
        break;
      default:
        if (typeof message.text === 'string') addMessage(message.text);
    }
  }

  function connect() {
    if (connected()) {
      if (listening) send({ type: 'mic', enabled: false });
      listening = false;
      renderListening();
      socket.close(1000, 'Disconnected by user');
      return;
    }
    if (socket && socket.readyState === WebSocket.CONNECTING) return;
    try {
      socket = new WebSocket(WS_URL);
      setConnection('connecting');
      socket.addEventListener('open', () => { setConnection('connected'); showToast('Connected to Testudeo.'); });
      socket.addEventListener('message', handleMessage);
      socket.addEventListener('close', () => {
        listening = false;
        renderListening();
        setConnection('disconnected');
      });
      socket.addEventListener('error', () => {
        setConnection('disconnected');
        showToast('Could not connect. Check that the robot app is available at ' + WS_URL);
      });
    } catch (error) {
      setConnection('disconnected');
      showToast('WebSocket connection failed: ' + error.message);
    }
  }

  connectButton.addEventListener('click', connect);
  micButton.addEventListener('click', () => {
    if (!$('#presentation-view').classList.contains('active')) return;
    if (!connected()) { showToast('Connect to the robot before enabling its microphone.'); return; }
    const next = !listening;
    if (send({ type: 'mic', enabled: next })) setListening(next);
  });

  $('#chat-form').addEventListener('submit', (event) => {
    event.preventDefault();
    const input = $('#chat-input');
    const text = input.value.trim();
    if (!text) return;
    if (send({ type: 'chat', message: text })) {
      addMessage(text, 'user');
      input.value = '';
    }
  });

  document.querySelectorAll('.nav-item[data-tab]').forEach((button) => {
    button.addEventListener('click', () => {
      const target = button.dataset.tab;
      if (target === 'pomodoro' && listening) {
        if (connected()) send({ type: 'mic', enabled: false });
        listening = false;
        renderListening();
      }
      document.querySelectorAll('.nav-item').forEach((item) => {
        item.classList.toggle('active', item === button);
        if (item === button) item.setAttribute('aria-current', 'page');
        else item.removeAttribute('aria-current');
      });
      $('#presentation-view').classList.toggle('active', target === 'presentation');
      $('#presentation-view').hidden = target !== 'presentation';
      $('#pomodoro-view').classList.toggle('active', target === 'pomodoro');
      $('#pomodoro-view').hidden = target !== 'pomodoro';
      $('#topbar-title').textContent = target === 'pomodoro' ? 'Pomodoro' : 'Presentation';
    });
  });

  function renderTimer() {
    const total = timerPhase === 'focus' ? FOCUS_SECONDS : BREAK_SECONDS;
    const minutes = Math.floor(remaining / 60).toString().padStart(2, '0');
    const seconds = (remaining % 60).toString().padStart(2, '0');
    $('#timer-display').textContent = `${minutes}:${seconds}`;
    $('#timer-phase').textContent = timerPhase === 'focus' ? 'FOCUS TIME' : 'BREAK TIME';
    $('#timer-subtitle').textContent = timerPhase === 'focus' ? 'A little progress adds up.' : 'Rest is part of the work.';
    $('#pomodoro-view').classList.toggle('timer-break', timerPhase === 'break');
    $('#timer-progress').style.strokeDasharray = String(CIRCUMFERENCE);
    $('#timer-progress').style.strokeDashoffset = String(CIRCUMFERENCE * (1 - remaining / total));
    $('#start-timer span').textContent = timerRunning ? 'Pause' : (timerPhase === 'focus' ? 'Start focus' : 'Start break');
    $('#start-timer .play-icon').innerHTML = timerRunning ? '<path d="M7 5h4v14H7zM14 5h4v14h-4z"></path>' : '<path d="m8 5 12 7-12 7V5Z"></path>';
    $('#up-next-title').textContent = timerPhase === 'focus' ? 'A little break' : 'Back to focus';
    $('#up-next-copy').textContent = timerPhase === 'focus' ? 'After 20 minutes, take 5 to stretch, sip some water, or just breathe.' : 'When your 5-minute break ends, you’ll be ready for another 20-minute focus session.';
    $('#up-next-time').innerHTML = `${timerPhase === 'focus' ? '05' : '20'} <span>MIN</span>`;
    document.title = `${minutes}:${seconds} · ${timerPhase === 'focus' ? 'Focus' : 'Break'} — Orbit`;
  }

  function switchPhase() {
    timerPhase = timerPhase === 'focus' ? 'break' : 'focus';
    remaining = timerPhase === 'focus' ? FOCUS_SECONDS : BREAK_SECONDS;
    renderTimer();
    showToast(timerPhase === 'break' ? 'Focus session complete. Take a 5-minute break.' : 'Break complete. Ready for another 20 minutes?');
  }

  $('#start-timer').addEventListener('click', () => {
    timerRunning = !timerRunning;
    if (timerRunning) {
      timerInterval = setInterval(() => {
        if (remaining > 0) remaining -= 1;
        if (remaining <= 0) {
          clearInterval(timerInterval);
          timerRunning = false;
          switchPhase();
        }
        renderTimer();
      }, 1000);
    } else {
      clearInterval(timerInterval);
    }
    renderTimer();
  });

  $('#reset-timer').addEventListener('click', () => {
    clearInterval(timerInterval);
    timerRunning = false;
    timerPhase = 'focus';
    remaining = FOCUS_SECONDS;
    renderTimer();
  });

  $('#skip-timer').addEventListener('click', () => {
    clearInterval(timerInterval);
    timerRunning = false;
    switchPhase();
  });

  window.addEventListener('beforeunload', () => {
    if (connected() && listening) send({ type: 'mic', enabled: false });
    if (socket) socket.close();
  });

  renderListening();
  renderTimer();
})();
