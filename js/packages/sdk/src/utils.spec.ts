import {Observable} from 'rxjs';
import {
    getFileDesc,
    getFileExt,
    timeout,
    toCamelCase,
    toSnakeCase
} from './utils';


describe('getFileDesc()', () => {
    it('should return the file {name, path, size, type}', () => {
        const file = new File([''], 'test.json');
        const {name, path, size, type} = getFileDesc(file);
        expect(name).toBe('test');
        expect(path).toBe(file.name);
        expect(size).toBe(file.size);
        expect(type).toBe('json');
    });
});

describe('getFileExt()', () => {
    it('should aggregate the file type/extension from the file name', () => {
        const file = new File(['{}'], 'test.json', {type: 'application/json'});
        const type = getFileExt(file);
        expect(type).toBe('json');
    });

    // We assume that everything after the first . in the filename is the file extension
    it('should return everything after the last dot in the filename', () => {
        const file = new File(['{}'], 'test.mesh.xml', {type: 'application/json'});
        const type = getFileExt(file);
        expect(type).toEqual('xml');
    });

    it('should return an empty string filename has not extension', () => {
        const file = new File(['{}'], 'test', {type: 'application/json'});
        const type = getFileExt(file);
        expect(type).toBe('');
    });
});

describe('toCamelCase()', () => {
    it('should convert object keys to camelcase recursive', () => {
        const json = {
            pings: [{
                foo_bar: {
                    bar_foo: true
                }
            }]
        };

        expect(toCamelCase(json))
            .toEqual({
                pings: [{
                    fooBar: {
                        barFoo: true
                    }
                }]
            });
    });

    it('should skip primitives', () => {
        expect(toCamelCase(true))
            .toBe(true);
        expect(toCamelCase(1))
            .toBe(1);
        expect(toCamelCase('abc'))
            .toBe('abc');
        expect(toCamelCase(null))
            .toBeNull();
    });
});

describe('toSnakeCase()', () => {
    it('should convert object keys to snakecase recursive', () => {
        const json = {
            pings: [{
                fooBar: {
                    barFoo: [1, 2, 3]
                }
            }]
        };

        expect(toSnakeCase(json))
            .toEqual({
                pings: [{
                    foo_bar: {
                        bar_foo: [1, 2, 3]
                    }
                }]
            });
    });

    it('should skip primitives', () => {
        expect(toSnakeCase(true))
            .toBe(true);
        expect(toSnakeCase(1))
            .toBe(1);
        expect(toSnakeCase('abc'))
            .toBe('abc');
        expect(toSnakeCase(null))
            .toBeNull();
    });
});

describe('timeout()', () => {
    beforeEach(() => {
        jest.useFakeTimers();
    });
    afterEach(() => {
        jest.useRealTimers();
    });

    it('should return an Observable', () => {
        const obs = timeout(0);
        expect(obs).toBeInstanceOf(Observable);
    });

    it('should behave as setTimeout()', done => {
        timeout(100)
            .subscribe(() => {
                done();
            });
        jest.runOnlyPendingTimers();
    });
});
