// Crypto
// https://developer.mozilla.org/en-US/docs/Web/API/Crypto
const crypto = require('@trust/webcrypto');
global.crypto = crypto;

// Mock WebSocket
const {WebSocket} = require('mock-socket');
global.WebSocket = WebSocket;
