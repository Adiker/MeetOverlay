// This is a heuristic, not a supported Google Meet API. Fail closed.
(() => {
  const mic = /\b(?:microphone|mic)\b|mikrofon/iu;
  const leave = /\b(?:leave (?:the )?call|leave (?:the )?meeting)\b|opuść (?:rozmowę|spotkanie)/iu;
  const turnOn = /\b(?:turn on|unmute)\b|(?:^|\s)włącz|wyłącz wyciszenie/iu;
  const turnOff = /\b(?:turn off|mute)\b|(?:^|\s)wyłącz|wycisz/iu;
  function interpret(controls) {
    const active = controls.filter(c => c.visible && !c.disabled);
    if (!active.some(c => leave.test(c.label))) return 'unknown';
    const candidates = active.filter(c => mic.test(c.label));
    if (candidates.length !== 1) return 'unknown';
    const c = candidates[0];
    const on = turnOn.test(c.label), off = turnOff.test(c.label);
    if (on === off) return 'unknown';
    const state = on ? 'muted' : 'active';
    if (c.isMuted != null && (!['true', 'false'].includes(c.isMuted) ||
        (c.isMuted === 'true') !== (state === 'muted'))) return 'unknown';
    return state;
  }
  globalThis.meetOverlayInterpret = interpret;
})();
