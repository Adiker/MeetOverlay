# Architecture and trust boundaries

The content script observes DOM mutations on meet.google.com and interprets a
single visible microphone control only when an accessible leave-call control is
also present. This heuristic is undocumented by Google. One Polish-language live meeting
confirmed its labels and attributes; other configurations remain unvalidated. Unknown or conflicting evidence produces `unknown`. No audio,
meeting URL, meeting title or participant data is sent to the native application.

A background service worker maintains one port per Meet tab and one Native
Messaging connection per browser profile. Multiple possible meeting tabs deliberately produce unknown, including
pre-join tabs; the explicit Meet home paths are excluded. Each renderer receives a liveness
request every 15 seconds; after 45 seconds without a reply the state is unknown.
The host validates native framing and forwards JSON over a same-user Unix socket
in XDG_RUNTIME_DIR. The Qt application combines profiles: exactly one reported
Meet tab is required. A 50-second expiry prevents stale state when a process
stalls. A lock prevents two overlay servers. The host exits on EOF or disconnect;
the extension retries with capped backoff while a Meet tab is open.

QML draws a click-through LayerShellQt surface. Settings are stored in an XDG
INI file, watched for changes and shared between the settings process and the
running overlay. Screen names are saved; an unavailable screen falls back to
primary. Coordinates are logical Wayland coordinates. Systemd and desktop
integration files are installed but not enabled.

Local same-user processes are within the trust boundary: Chromium extension IDs
and Unix socket permissions do not authenticate against a malicious process
running as the same user. See Chromium's Native Messaging documentation.
