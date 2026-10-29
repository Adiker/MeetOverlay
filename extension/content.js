(() => {
  let port, last = null, scheduled = false, reconnect;
  function snapshot() {
    const controls = [...document.querySelectorAll('button,[role="button"]')].map(el => ({
      label: el.getAttribute('aria-label') || '',
      isMuted: el.getAttribute('data-is-muted'),
      visible: !!el.getClientRects().length && getComputedStyle(el).visibility === 'visible',
      disabled: el.hasAttribute('disabled') || el.getAttribute('aria-disabled') === 'true'
    }));
    return globalThis.meetOverlayInterpret(controls);
  }
  function update(force = false) {
    scheduled = false;
    const state = snapshot();
    if (force || state !== last) {
      last = state;
      try { port?.postMessage({state}); } catch (_) { /* disconnected */ }
    }
  }
  function schedule() {
    if (!scheduled) { scheduled = true; queueMicrotask(() => update()); }
  }
  function connect() {
    try {
      port = chrome.runtime.connect({name: 'meet-state'});
      port.onMessage.addListener(message => { if (message === 'refresh') update(true); });
      port.onDisconnect.addListener(() => {
        port = null; last = null;
        clearTimeout(reconnect); reconnect = setTimeout(connect, 5000);
      });
      update(true);
    } catch (_) { clearTimeout(reconnect); reconnect = setTimeout(connect, 5000); }
  }
  const observer = new MutationObserver(schedule);
  observer.observe(document.documentElement, {subtree: true, childList: true, attributes: true,
    attributeFilter: ['aria-label', 'aria-disabled', 'disabled', 'data-is-muted', 'class', 'style', 'hidden']});
  addEventListener('pagehide', () => { observer.disconnect(); clearTimeout(reconnect); port?.disconnect(); }, {once: true});
  connect();
})();
