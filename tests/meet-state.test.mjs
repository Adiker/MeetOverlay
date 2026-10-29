import { test } from 'node:test';
import assert from 'node:assert/strict';
import { interpret } from '../research/meet-state.mjs';

const leave = {label: 'Leave call', visible: true};
const mic = (label, isMuted) => ({label, isMuted, visible: true});
test('requires evidence of a joined call', () => {
  assert.equal(interpret([mic('Turn on microphone', 'true')]), 'unknown');
});
test('recognizes inverse action labels', () => {
  assert.equal(interpret([leave, mic('Turn on microphone', 'true')]), 'muted');
  assert.equal(interpret([leave, mic('Turn off microphone', 'false')]), 'active');
});
test('fails closed on conflicts and ambiguous controls', () => {
  assert.equal(interpret([leave, mic('Turn on microphone', 'false')]), 'unknown');
  assert.equal(interpret([leave, mic('Turn on microphone', 'true'), mic('Turn off microphone', 'false')]), 'unknown');
  assert.equal(interpret([leave, mic('Microphone', 'true')]), 'unknown');
  assert.equal(interpret([leave, mic('Turn on microphone', 'unexpected')]), 'unknown');
});
test('ignores hidden and disabled controls', () => {
  assert.equal(interpret([leave, {...mic('Turn on microphone', 'true'), visible: false}]), 'unknown');
  assert.equal(interpret([leave, {...mic('Turn on microphone', 'true'), disabled: true}]), 'unknown');
});
