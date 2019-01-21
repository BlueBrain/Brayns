import {getHostFromUrl, parseQueryParams} from './url';

describe('getHostFromUrl()', () => {
    const href = window.location.href;
    const jsdom: any = (global as any).jsdom;

    afterEach(() => {
        jsdom.reconfigure({
            url: href
        });
    });

    it('should get the host from query params if it exists', () => {
        const host = '127.0.0.1:8080';
        jsdom.reconfigure({
            url: `http://localhost:8765?host=${host}`
        });
        const h = getHostFromUrl();
        expect(h).toEqual(host);
    });

    // TODO: This does not seem to testable anymore, figure out why
    it.skip('should get the host from env var if it exists', () => {
        const host = '127.0.0.1:8080';
        process.env = {
            ...process.env,
            REACT_APP_RENDERER_SERVICE_URL: host
        };
        const h = getHostFromUrl();
        expect(h).toEqual(host);
        delete process.env.REACT_APP_RENDERER_SERVICE_URL;
    });

    it('should fallback to localhost', () => {
        const host = getHostFromUrl();
        expect(host).toEqual('127.0.0.1:8200');
    });
});

describe('parseQueryParams()', () => {
    it('should get query params from url', () => {
        const host = '127.0.0.1:8080';
        const params = parseQueryParams(`http://localhost:8080?host=${host}`);
        expect(params).toEqual({host});
    });

    it('should return an empty object if the url has no query params', () => {
        const host = '127.0.0.1:8080';
        const params = parseQueryParams(host);
        expect(params).toEqual({});
    });

    it('should return an empty object if URL is not supported', () => {
        const jsdom: any = (global as any).jsdom;
        // Set URL to undefined
        const URL = jsdom.window.URL;
        jsdom.window.URL = undefined as any;

        const host = '127.0.0.1:8080';
        const params = parseQueryParams(host);

        // Set URL back to what it was
        jsdom.window.URL = URL;

        expect(params).toEqual({});
    });
});
