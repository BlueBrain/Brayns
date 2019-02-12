// tslint:disable: no-empty
import {
    isFunction,
    isString,
    noop,
    pick
} from 'lodash';

import {Server} from 'mock-socket';

import {
    isJsonRpcNotification,
    isJsonRpcRequest,
    JsonRpcNotification,
    JsonRpcRequest,
    Notification
} from 'rockets-client';

import {Observable} from 'rxjs';
import {skip, take} from 'rxjs/operators';

import {appendWsPath, Client} from './client';
import {
    CHUNK,
    IMAGE_JPEG,
    INSPECT,
    SET_ANIMATION_PARAMS,
    SET_APP_PARAMS,
    UPLOAD_MODEL
} from './constants';
import {BinaryParams, ChunkParams} from './types';

import {
    fromJson,
    toJson,
    toResponseErrorJson,
    toResponseJson
} from './testing';


const host = 'ws://myhost';
const wsHost = appendWsPath(host); // '/ws' is appended at the end of host


describe('Client', () => {
    describe('Connection', () => {
        let mockServer: Server;
        beforeEach(() => {
            jest.useFakeTimers();
            mockServer = new Server(wsHost);
        });
        afterEach(() => {
            mockServer.stop(noop);
            jest.runOnlyPendingTimers();
            jest.useRealTimers();
        });

        it('should connect to the Brayns server', done => {
            // @ts-ignore
            const brayns = new Client(host);

            mockServer.on('connection', () => {
                const [client] = mockServer.clients();
                expect(client.url.indexOf(wsHost) !== -1).toBe(true);
                done();
            });

            jest.runOnlyPendingTimers();
        });

        it('should try to reconnect if socket is closed', () => {
            // @ts-ignore
            const brayns = new Client(host);

            let connections = 0;
            mockServer.on('connection', socket => {
                if (connections === 0) {
                    socket.close();
                    jest.runOnlyPendingTimers();
                }
                connections++;
            });

            jest.runOnlyPendingTimers();

            // Advance time more than 5s (current reconnect timeout)
            jest.advanceTimersByTime(6000);

            expect(connections).toBe(2);
        });

        it('should stop trying to reconnect if it connected after a retry', () => {
            // @ts-ignore
            const brayns = new Client(host);

            let connections = 0;
            mockServer.on('connection', socket => {
                if (connections === 0) {
                    socket.close();
                    jest.runOnlyPendingTimers();
                }
                connections++;
            });

            jest.runOnlyPendingTimers();

            // Advance time more than 2 * 5s (current reconnect timeout)
            jest.advanceTimersByTime(12000);

            expect(connections).toBe(2);
        });
    });

    describe('{ready}', () => {
        let mockServer: Server;
        beforeEach(() => {
            mockServer = new Server(wsHost);
        });
        afterEach(done => {
            mockServer.stop(done);
        });

        it('should emit false by default', done => {
            const brayns = new Client(host);
            brayns.ready.pipe(take(1))
                .subscribe(ready => {
                    expect(ready).toBe(false);
                    done();
                });
        });

        it('should emit true when the socket establishes a connection with the ws server', done => {
            const brayns = new Client(host);
            brayns.ready.pipe(skip(1), take(1))
                .subscribe(ready => {
                    expect(ready).toBe(true);
                    done();
                });
        });

        it('should emit false when the socket connection with the ws server is closed', done => {
            const brayns = new Client(host);

            brayns.ready.pipe(skip(2), take(1))
                .subscribe(ready => {
                    expect(ready).toBe(false);
                    done();
                });

            mockServer.on('connection', socket => {
                socket.close(0, 'Why not?');
            });
        });

        it('should emit false when the socket connection with the ws server errored', done => {
            const brayns = new Client(host);

            brayns.ready.pipe(skip(2), take(1))
                .subscribe(ready => {
                    expect(ready).toBe(false);
                    done();
                });

            mockServer.on('connection', socket => {
                mockServer.close({
                    code: 1005,
                    reason: 'Why not?',
                    wasClean: false
                });
            });
        });
    });

    describe('.observe()', () => {
        let mockServer: Server;
        beforeEach(() => {
            mockServer = new Server(wsHost);
        });
        afterEach(done => {
            mockServer.stop(done);
        });

        it('should return an Observable', () => {
            const brayns = new Client(host);
            const observer = brayns.observe(SET_ANIMATION_PARAMS);
            expect(observer instanceof Observable).toBe(true);
        });

        it('should emit notifications from upstream if their methods are the same as the events we are observing', done => {
            const brayns = new Client(host);
            const notification = new Notification(SET_APP_PARAMS, {});

            brayns.observe(SET_APP_PARAMS)
                .subscribe(data => {
                    expect(data).toEqual(notification.params);
                    done();
                });

            mockServer.on('connection', socket => {
                const json = toJson(notification);
                socket.send(json);
            });
        });

        it('should convert snakecase keys to camelcase', done => {
            const brayns = new Client(host);

            const params = {
                ping_pong: true
            };
            const notification = new Notification(SET_APP_PARAMS, params);

            brayns.observe(SET_APP_PARAMS)
                .subscribe(data => {
                    expect(data).toEqual({
                        pingPong: true
                    });
                    done();
                });

            mockServer.on('connection', socket => {
                const json = toJson(notification);
                socket.send(json);
            });
        });

        it('should only receive Blob values for IMAGE_JPEG', done => {
            const brayns = new Client(host);
            brayns.observe(IMAGE_JPEG)
                .subscribe(data => {
                    expect(data).toBeInstanceOf(Blob);
                    done();
                });

            mockServer.on('connection', socket => {
                const notification = new Notification(SET_APP_PARAMS);
                const json = toJson(notification);
                socket.send(json);
                const blob = new Blob();
                socket.send(blob);
            });
        });
    });

    describe('.notify()', () => {
        let mockServer: Server;
        beforeEach(() => {
            mockServer = new Server(wsHost);
        });
        afterEach(done => {
            mockServer.stop(done);
        });

        it('should send a notification', done => {
            const brayns = new Client(host);

            mockServer.on('connection', socket => {
                // TODO: (socket as any) is due to https://github.com/thoov/mock-socket/issues/224,
                // remove when fixed
                (socket as any).on('message', (data: any) => {
                    const json = fromJson<JsonRpcNotification>(data);
                    expect(isJsonRpcNotification(json)).toBe(true);
                    expect(json.method).toBe(SET_ANIMATION_PARAMS);
                    expect(json.params).toEqual({
                        ping: true
                    });
                    done();
                });
            });

            brayns.notify(SET_ANIMATION_PARAMS, {
                ping: true
            } as any);
        });

        it('should convert params to snakecase', done => {
            const brayns = new Client(host);

            mockServer.on('connection', socket => {
                // TODO: (socket as any) is due to https://github.com/thoov/mock-socket/issues/224,
                // remove when fixed
                (socket as any).on('message', (data: any) => {
                    const json = fromJson<JsonRpcNotification>(data);
                    expect(json.params).toEqual({
                        ping_pong: true
                    });
                    done();
                });

            });

            brayns.notify(SET_ANIMATION_PARAMS, {
                pingPong: true
            } as any);
        });
    });

    describe('.request()', () => {
        let mockServer: Server;
        beforeEach(() => {
            mockServer = new Server(wsHost);
        });
        afterEach(done => {
            mockServer.stop(done);
        });

        it('should return a RequestTask', done => {
            const brayns = new Client(host);

            mockServer.on('connection', () => {
                const task = brayns.request(INSPECT, [0, 1]);

                expect(isJsonRpcRequest(task.request)).toBe(true);
                expect(isFunction(task.then)).toBe(true);
                expect(isFunction(task.on)).toBe(true);
                expect(isFunction(task.cancel)).toBe(true);

                done();
            });
        });

        it('should send a request', done => {
            const brayns = new Client(host);

            mockServer.on('connection', socket => {
                // TODO: (socket as any) is due to https://github.com/thoov/mock-socket/issues/224,
                // remove when fixed
                (socket as any).on('message', (data: any) => {
                    const json = fromJson<JsonRpcRequest>(data);
                    expect(isJsonRpcRequest(json)).toBe(true);
                    expect(json.id).toBeDefined();
                    expect(json.method).toBe(INSPECT);
                    expect(json.params).toEqual({
                        ping: true
                    });

                    const response = toResponseJson(json, true);
                    socket.send(response);
                });
            });

            brayns.request(INSPECT, {ping: true} as any)
                .then(() => {
                    done();
                });
        });

        it('should send params with object keys converted to snakecase', done => {
            const brayns = new Client(host);

            mockServer.on('connection', socket => {
                // TODO: (socket as any) is due to https://github.com/thoov/mock-socket/issues/224,
                // remove when fixed
                (socket as any).on('message', (data: any) => {
                    const json = fromJson<JsonRpcRequest>(data);
                    expect(json.params).toEqual({
                        ping_pong: true
                    });

                    const response = toResponseJson(json, true);
                    socket.send(response);
                });
            });

            brayns.request(INSPECT, {pingPong: true} as any)
                .then(() => {
                    done();
                });
        });

        it('should receive a response with object keys converted to camelcase', done => {
            const brayns = new Client(host);

            mockServer.on('connection', socket => {
                // TODO: (socket as any) is due to https://github.com/thoov/mock-socket/issues/224,
                // remove when fixed
                (socket as any).on('message', (data: any) => {
                    const json = fromJson<JsonRpcRequest>(data);
                    const response = toResponseJson(json, {
                        just_pinged: true
                    });
                    socket.send(response);
                });
            });

            brayns.request(INSPECT, {ping: true} as any)
                .then(res => {
                    expect(res).toEqual({
                        justPinged: true
                    });
                    done();
                });
        });
    });

    describe('.upload()', () => {
        let mockServer: Server;
        beforeEach(() => {
            mockServer = new Server(wsHost);
        });
        afterEach(done => {
            mockServer.stop(done);
        });

        it('should read a file and send it over the ws in chunks', done => {
            const brayns = new Client(host);

            const data = {ping: true};
            const file = generateFile('test.json', data);
            const chunkSize = 7;
            const expectedLength = Math.round(file.size / chunkSize);

            let request: JsonRpcRequest<BinaryParams>;
            const notifications: Array<JsonRpcNotification<ChunkParams>> = [];
            const chunks: ArrayBuffer[] = [];

            mockServer.on('connection', socket => {
                // TODO: (socket as any) is due to https://github.com/thoov/mock-socket/issues/224,
                // remove when fixed
                (socket as any).on('message', (data: any) => {
                    if (isString(data)) {
                        const json = fromJson<JsonRpcRequest<BinaryParams> | JsonRpcNotification<ChunkParams>>(data);

                        if (isJsonRpcRequest(json)) {
                            request = json;
                        } else if (isJsonRpcNotification(json)) {
                            notifications.push(json);
                        }
                    } else {
                        chunks.push(data);

                        if (chunks.length === expectedLength) {
                            // Received all chunks,
                            // respond so the request can complete
                            const response = toResponseJson(request, true);
                            socket.send(response);
                        }
                    }
                });
            });

            brayns.upload({
                file,
                chunkSize
            }).then(() => {
                expect(notifications).toHaveLength(expectedLength);
                expect(notifications.every(({method, params}) => method === CHUNK && isString(params!.id))).toBe(true);

                expect(chunks).toHaveLength(expectedLength);
                expect(chunks.every(chunk => chunk instanceof ArrayBuffer)).toBe(true);
                const json = decodeChunks(chunks);
                expect(json).toEqual(data);

                const {method, params, id} = request;
                expect(id).toBeDefined();
                expect(method).toBe(UPLOAD_MODEL);
                const {chunks_id} = params! as any;
                expect(notifications.every(({method, params}) => params!.id === chunks_id)).toBe(true);
                expect(isString(chunks_id)).toBe(true);
                expect(pick(params, ['name', 'path', 'type', 'size'])).toEqual({
                    name: 'test',
                    path: 'test.json',
                    type: 'json',
                    size: file.size
                });

                done();
            });
        });

        it('should send the chunk notification and chunks sequentially', done => {
            const brayns = new Client(host);

            const data = {ping: true};
            const file = generateFile('test.json', data);

            const chunkSize = 7;
            const expectedLength = Math.round(file.size / chunkSize);

            const messages: any[] = [];
            let request: JsonRpcRequest<BinaryParams>;
            const chunks: ArrayBuffer[] = [];

            mockServer.on('connection', socket => {
                // TODO: (socket as any) is due to https://github.com/thoov/mock-socket/issues/224,
                // remove when fixed
                (socket as any).on('message', (data: any) => {
                    messages.push(data);

                    if (isString(data)) {
                        const json = fromJson<JsonRpcRequest<BinaryParams> | JsonRpcNotification<ChunkParams>>(data);
                        if (isJsonRpcRequest(json)) {
                            request = json;
                        }
                    } else {
                        chunks.push(data);

                        if (chunks.length === expectedLength) {
                            // Received all chunks,
                            // respond so the request can complete
                            const response = toResponseJson(request, true);
                            socket.send(response);
                        }
                    }
                });
            });

            brayns.upload({
                file,
                chunkSize
            }).then(() => {
                const [requestJson, ...objects] = messages;

                const request = fromJson(requestJson);
                expect(isJsonRpcRequest(request)).toBe(true);

                const notifications = objects.filter((obj, index) => index % 2 === 0)
                    .map(json => fromJson<JsonRpcNotification<ChunkParams>>(json));
                const chunks = objects.filter((obj, index) => index % 2 !== 0);

                expect(notifications).toHaveLength(expectedLength);
                expect(notifications.every(({method, params}) => method === CHUNK && isString(params!.id))).toBe(true);

                expect(chunks).toHaveLength(expectedLength);
                expect(chunks.every(chunk => chunk instanceof ArrayBuffer)).toBe(true);
                const json = decodeChunks(chunks);
                expect(json).toEqual(data);

                done();
            });
        });

        it('should allow user to set other params {type, name, visible, boundingBox}', done => {
            const brayns = new Client(host);

            const data = {ping: true};
            const name = 'different.json';
            const visible = false;
            const boundingBox = true;
            const type = '*.assbin';
            const file = generateFile('test.json', data);

            const chunkSize = 7;
            const expectedLength = Math.round(file.size / chunkSize);

            let request: JsonRpcRequest<BinaryParams>;
            const notifications: Array<JsonRpcNotification<ChunkParams>> = [];
            const chunks: ArrayBuffer[] = [];

            mockServer.on('connection', socket => {
                // TODO: (socket as any) is due to https://github.com/thoov/mock-socket/issues/224,
                // remove when fixed
                (socket as any).on('message', (data: any) => {
                    if (isString(data)) {
                        const json = fromJson<JsonRpcRequest<BinaryParams> | JsonRpcNotification<ChunkParams>>(data);

                        if (isJsonRpcRequest(json)) {
                            request = json;
                        } else if (isJsonRpcNotification(json)) {
                            notifications.push(json);
                        }
                    } else {
                        chunks.push(data);

                        if (chunks.length === expectedLength) {
                            // Received all chunks,
                            // respond so the request can complete
                            const response = toResponseJson(request, true);
                            socket.send(response);
                        }
                    }
                });
            });

            brayns.upload({
                file,
                chunkSize,
                boundingBox,
                name,
                type,
                visible
            }).then(() => {
                expect(notifications).toHaveLength(expectedLength);
                expect(notifications.every(({method, params}) => method === CHUNK && isString(params!.id))).toBe(true);

                expect(chunks).toHaveLength(expectedLength);
                expect(chunks.every(chunk => chunk instanceof ArrayBuffer)).toBe(true);
                const json = decodeChunks(chunks);
                expect(json).toEqual(data);

                const {method, params, id} = request;
                expect(id).toBeDefined();
                expect(method).toBe(UPLOAD_MODEL);
                const {chunks_id} = params! as any;
                expect(isString(chunks_id)).toBe(true);
                expect(pick(params! as any, ['name', 'path', 'type', 'size', 'bounding_box', 'visible'])).toEqual({
                    name,
                    type,
                    bounding_box: boundingBox,
                    visible,
                    path: file.name,
                    size: file.size
                });

                done();
            });
        });

        it('should stop uploading if the request for binary receive fails', async done => {
            const brayns = new Client(host);

            const data = {ping: true};
            const file = generateFile('test.json', data);

            const chunkSize = 0.1;
            const expectedLength = Math.round(file.size / chunkSize);

            const notifications: Array<JsonRpcNotification<ChunkParams>> = [];
            const chunks: ArrayBuffer[] = [];

            mockServer.on('connection', socket => {
                // TODO: (socket as any) is due to https://github.com/thoov/mock-socket/issues/224,
                // remove when fixed
                (socket as any).on('message', (data: any) => {
                    if (isString(data)) {
                        const json = fromJson<JsonRpcRequest<BinaryParams> | JsonRpcNotification<ChunkParams>>(data);

                        if (isJsonRpcRequest(json)) {
                            const response = toResponseErrorJson(json, {
                                code: -1,
                                message: 'Oops :('
                            });
                            socket.send(response);
                        } else if (isJsonRpcNotification(json)) {
                            notifications.push(json);
                        }
                    } else {
                        chunks.push(data);
                    }
                });
            });

            brayns.upload({
                file,
                chunkSize
            }).then(() => {
                done.fail('Request should have failed');
            }, () => {
                expect(chunks).not.toHaveLength(expectedLength);
                done();
            });
        });
    });
});

describe('appendWsPath()', () => {
    it('should append \'ws\' to a path', () => {
        const url = 'test';
        const urlWithWs = appendWsPath(url);
        expect(urlWithWs).toEqual(`${url}/ws`);
    });

    it('should omit trailing slash', () => {
        const url = 'test/';
        const urlWithWs = appendWsPath(url);
        expect(urlWithWs).toEqual('test/ws');
    });
});


function generateFile(name: string, data: any, type = 'application/json'): File {
    const json = JSON.stringify(data);
    const blob = new Blob([json], {type});
    const file = new File([blob], name, {type});
    return file;
}

function decodeChunks(chunks: ArrayBuffer[]) {
    const strChunks = chunks.map(chunk => abToStr(chunk));
    const jsonStr = strChunks.reduce((acc, chunk) => acc + chunk);
    try {
        const json = JSON.parse(jsonStr);
        return json;
    } catch {}
}

function abToStr(buffer: ArrayBuffer): string {
    const bytes = new Uint8Array(buffer);
    return String.fromCharCode.apply(null, bytes as any);
}
