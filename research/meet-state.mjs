/**
 * Experimental, fail-closed interpretation of Meet controls.
 * This module has not been validated against a live meeting.
 * Inputs are normalized snapshots, never page content or meeting identifiers.
 */
const MICROPHONE = /\b(?:microphone|mic)\b|mikrofon/iu;
const LEAVE = /\b(?:leave (?:the )?call|leave (?:the )?meeting)\b|opuść (?:rozmowę|spotkanie)/iu;
const TURN_ON = /\b(?:turn on|unmute)\b|włącz|wyłącz wyciszenie/iu;
const TURN_OFF = /\b(?:turn off|mute)\b|wyłącz|wycisz/iu;

export function interpret(controls) {
  const visible = controls.filter(c => c.visible && !c.disabled);
  // A pre-join microphone control also exists. Require evidence of a joined call.
  if (!visible.some(c => LEAVE.test(c.label))) return 'unknown';
  const candidates = visible.filter(c => MICROPHONE.test(c.label));
  if (candidates.length !== 1) return 'unknown';
  const control = candidates[0];
  const on = TURN_ON.test(control.label);
  const off = TURN_OFF.test(control.label);
  if (on === off) return 'unknown';
  const fromLabel = on ? 'muted' : 'active';
  if (control.isMuted != null) {
    if (!['true', 'false'].includes(control.isMuted)) return 'unknown';
    if ((control.isMuted === 'true') !== (fromLabel === 'muted')) return 'unknown';
  }
  return fromLabel;
}
