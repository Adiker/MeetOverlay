import {test} from 'node:test';
import assert from 'node:assert/strict';
import vm from 'node:vm';
import {readFileSync} from 'node:fs';
function event() { const listeners=[]; return {addListener(fn){listeners.push(fn)}, emit(...a){for(const fn of listeners) fn(...a)}}; }
function setup() {
  const connect=event(), native=[]; let tick, now=0;
  const chrome={runtime:{onConnect:connect, connectNative(){const p={messages:[],onDisconnect:event(),onMessage:event(),postMessage(m){this.messages.push(m)},disconnect(){this.onDisconnect.emit()}}; native.push(p);return p}}};
  const context=vm.createContext({chrome,Date:{now:()=>now},setInterval(fn){tick=fn},setTimeout(){return 1},clearTimeout(){}});
  vm.runInContext(readFileSync(new URL('../extension/background.js',import.meta.url),'utf8'),context);
  function tab() { const p={name:'meet-state',sender:{frameId:0,tab:{}},onMessage:event(),onDisconnect:event(),postMessage(){}}; connect.emit(p); return p; }
  return {tab,native, advance(ms){now+=ms;tick()}};
}
test('tracks tab changes, ambiguity, disconnect and expiry',()=>{
  const x=setup(), a=x.tab(), host=x.native[0];
  a.onMessage.emit({state:'active'}); assert.equal(host.messages.at(-1).state,'active');
  const b=x.tab(); b.onMessage.emit({state:'muted'}); assert.equal(host.messages.at(-1).state,'unknown');
  b.onMessage.emit({state:'outside'}); assert.equal(host.messages.at(-1).state,'active');
  b.onDisconnect.emit(); assert.equal(host.messages.at(-1).state,'active');
  x.advance(46000); assert.equal(host.messages.at(-1).state,'unknown');
  a.onMessage.emit({state:'muted'}); assert.equal(host.messages.at(-1).state,'muted');
  a.onDisconnect.emit(); assert.equal(host.messages.at(-1).count,0);
});
