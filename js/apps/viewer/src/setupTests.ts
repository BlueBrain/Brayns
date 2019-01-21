// tslint:disable: max-classes-per-file no-empty only-arrow-functions object-literal-shorthand
// Setup test env
// https://github.com/facebookincubator/create-react-app/blob/master/packages/react-scripts/template/README.md#initializing-test-environment

// Required when running tests
import 'raf/polyfill';


// Polyfill Fetch and friends
// See https://github.com/jefflau/jest-fetch-mock for more details
// NOTE: This includes Response, Request, etc.
import 'isomorphic-fetch';


// Setup Enzyme
// http://airbnb.io/enzyme/#upgrading-from-enzyme-2x-or-react--16
// http://airbnb.io/enzyme/docs/installation/react-16.html#working-with-react-16
import {configure} from 'enzyme';
import Adapter from 'enzyme-adapter-react-16';

configure({
    adapter: new Adapter()
});


// Fixes issues with Material UI using Popper
// https://github.com/FezVrasta/popper.js/issues/478
jest.mock('popper.js', () => {
    // Need to import this in the context of the mock,
    // https://github.com/facebook/jest/issues/2567
    const PopperJs = require.requireActual('popper.js');

    return class Popper {
        static placements = PopperJs.placements;

        constructor() {
            return {
                destroy: jest.fn(),
                scheduleUpdate: jest.fn(),
                update: jest.fn()
            };
        }
    };
});


const g: any = global;
g.document = g.jsdom.window.document;


// Crypto
// https://developer.mozilla.org/en-US/docs/Web/API/Crypto
import crypto from '@trust/webcrypto';
g.crypto = crypto;


// Mock WebSocket
import {WebSocket} from 'mock-socket';
g.WebSocket = WebSocket;


// Mock localStorage
g.localStorage = {
    getItem(key: string) {
        return this[key];
    },
    setItem(key: string, value: any) {
        this[key] = value;
    },
    removeItem(key: string) {
        delete this[key];
    },
    clear() {
        for (const key of this) {
            delete this[key];
        }
    },
    [Symbol.iterator]: function*() {
        const keys = Object.keys(this)
            .filter(key => !['getItem', 'setItem', 'removeItem', 'clear'].includes(key));
        for (const key of keys) {
            yield key;
        }
    }
};


// Mock missing DOM API that popper.js wants
// https://github.com/FezVrasta/popper.js/blob/master/packages/popper/src/utils/findCommonOffsetParent.js#L27
g.document.createRange = () => ({
    setStart: () => {},
    setEnd: () => {},
    commonAncestorContainer: {
        nodeName: 'BODY'
    }
});


// Mock navigator.storage
g.navigator.storage = {
    estimate: jest.fn()
};


// Add missing URL methods
// TODO: Remove when https://github.com/jsdom/jsdom/issues/1721 is fixed
URL.revokeObjectURL = () => {};
URL.createObjectURL = (url: string) => {
    return url;
};

g.URL = URL;
