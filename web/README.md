# Orbit robot preview

A dependency-free browser frontend preview for the Testudeo Robot. It includes a presentation workspace with robot microphone controls and chat, plus a fixed-cycle Pomodoro timer.

## Run locally

From the repository root, serve the `web` directory over HTTP (rather than opening the HTML as a `file://` URL):

```sh
python3 -m http.server 8000 --directory web
```

Open <http://localhost:8000>. The UI is static and needs no package install or build step.

## Connect to the robot app

The preview connects to `ws://testudeo.local:8386/ws/stt` by default, matching the endpoint configured in the firmware. Ensure the browser can resolve `testudeo.local` and that the app server is reachable on port `8386`. If you host the app over HTTPS, serve the WebSocket endpoint securely (`wss://`) and update `WS_URL` near the top of `app.js`.

Presentation mode sends JSON microphone controls:

```json
{"type":"mic","enabled":true}
```

and

```json
{"type":"mic","enabled":false}
```

Chat sends JSON messages in this shape:

```json
{"type":"chat","message":"Hello, robot!"}
```

Incoming JSON with `type` `chat`, `message`, or `transcript` and a `message` or `text` field is shown in the conversation. A plain-text WebSocket message is also displayed. The existing ESP32 code shows the `mic` control shape, but its WebSocket client is the robot's connection to the app server; it does not itself implement browser-facing chat or fan-out. The app server must accept browser WebSocket connections and implement the `chat` message behavior for live chat to work end-to-end.

## Pomodoro

The timer uses a fixed 20-minute focus / 5-minute break cycle. Start, pause, reset, and skip are available. It runs locally in the browser and does not send timer commands to the robot.
