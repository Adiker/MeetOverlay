const tabs = new Map();
let nativePort = null, retry = null, retryDelay = 1000;
function snapshot() {
  const states = [...tabs.values()].map(v => v.state).filter(state => state !== 'outside');
  // Multiple Meet tabs are ambiguous, even when they report the same state.
  return {state: states.length === 1 ? states[0] : 'unknown', count: states.length};
}
function send() {
  if (!nativePort) return;
  try { nativePort.postMessage(snapshot()); } catch (_) { /* onDisconnect retries */ }
}
function connectNative() {
  if (nativePort || !tabs.size) return;
  clearTimeout(retry);
  try {
    const p = chrome.runtime.connectNative('org.meetoverlay.host');
    nativePort = p;
    p.onDisconnect.addListener(() => {
      if (nativePort !== p) return;
      void chrome.runtime.lastError;
      nativePort = null;
      if (tabs.size) { retry = setTimeout(connectNative, retryDelay); retryDelay = Math.min(retryDelay * 2, 30000); }
    });
    p.onMessage.addListener(() => {});
    send();
  } catch (_) { retry = setTimeout(connectNative, retryDelay); retryDelay = Math.min(retryDelay * 2, 30000); }
}
chrome.runtime.onConnect.addListener(port => {
  if (port.name !== 'meet-state' || port.sender?.frameId !== 0 || !port.sender?.tab) return;
  const entry = {state: 'unknown', seen: Date.now()};
  tabs.set(port, entry);
  port.onMessage.addListener(message => {
    if (!['muted', 'active', 'unknown', 'outside'].includes(message?.state)) return;
    entry.state = message.state; entry.seen = Date.now(); retryDelay = 1000; send();
  });
  port.onDisconnect.addListener(() => {
    tabs.delete(port); send();
    if (!tabs.size) { clearTimeout(retry); nativePort?.disconnect(); nativePort = null; }
  });
  connectNative(); send();
});
// Liveness probes ensure a stalled renderer cannot leave an old green/red state.
setInterval(() => {
  const now = Date.now();
  for (const [port, entry] of tabs) {
    if (now - entry.seen > 45000) entry.state = 'unknown';
    try { port.postMessage('refresh'); } catch (_) { entry.state = 'unknown'; }
  }
  send();
}, 15000);
