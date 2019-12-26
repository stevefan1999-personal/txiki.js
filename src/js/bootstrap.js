// tjs internal bootstrap.
//

import * as tjs from '@tjs/core';

globalThis.tjs = tjs;
globalThis.setTimeout = tjs.setTimeout;
globalThis.clearTimeout = tjs.clearTimeout;
globalThis.setInterval = tjs.setInterval;
globalThis.clearInterval = tjs.clearInterval;
globalThis.XMLHttpRequest = tjs.XMLHttpRequest;
globalThis.alert = tjs.alert;
globalThis.prompt = tjs.prompt;
globalThis.hrtime = tjs.hrtime;

Object.defineProperty(globalThis, 'global', {
    enumerable: true,
    configurable: true,
    writable: true,
    value: globalThis
});

Object.defineProperty(globalThis, 'window', {
    enumerable: true,
    configurable: true,
    writable: true,
    value: globalThis
});
