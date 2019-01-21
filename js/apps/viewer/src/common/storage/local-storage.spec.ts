import {noop} from 'lodash';
import LocalStorage from './local-storage';

describe('LocalStorage', () => {
    const ns = '@test';
    const key = 'test';
    const nsKey = `${ns}:${key}`;
    let storage: LocalStorage;

    beforeEach(() => {
        storage = new LocalStorage(ns);
    });

    afterEach(() => {
        // Cleanup localStorage after each test
        localStorage.clear();
    });

    describe('.set()', () => {
        it.skip('should prepend unique key and serialize to JSON', () => {
            const setItemSpy = spyOn(localStorage, 'setItem').and.callFake(noop);
            const data = {ping: true};
            storage.set(key, data);
            expect(setItemSpy).toHaveBeenCalledWith(nsKey, JSON.stringify(data));
        });

        it('should set items in storage', () => {
            const data = {ping: true};
            storage.set(key, data);
            const item = localStorage.getItem(nsKey);
            expect(item).toEqual(JSON.stringify(data));
        });
    });

    describe('.get()', () => {
        it.skip('should prepend unique key', () => {
            const getItemSpy = spyOn(localStorage, 'getItem').and.returnValue(null);
            storage.get(key);
            expect(getItemSpy).toHaveBeenCalledWith(nsKey);
        });

        it('should retrieve items from storage and deserialize', async () => {
            const data = {ping: true};
            const item = storage.set(key, data)
                .get(key);
            expect(item).toEqual(data);
        });
    });

    describe('.remove()', () => {
        it.skip('should prepend unique key', () => {
            const removeItemSpy = spyOn(localStorage, 'removeItem').and.callFake(noop);
            storage.remove(key);
            expect(removeItemSpy).toHaveBeenCalledWith(nsKey);
        });

        it('should remove item from storage', async () => {
            const data = {ping: true};
            // Let's set the item first and check it's there
            const item = await storage.set('test', data)
                .get(key);
            expect(item).toEqual(data);

            // Remove the item
            storage.remove(key);

            expect(await storage.get(key)).toBeNull();
        });
    });

    describe('.clear()', () => {
        it('should remove all items from storage (in current namespace)', async () => {
            // Set an item in localStorage directly
            localStorage.setItem(key, 'noop');

            const data = {ping: true};
            // Let's set the item first and check it's there
            const item = await storage.set(key, data)
                .get(key);
            expect(item).toEqual(data);

            // Remove the item
            storage.clear();

            expect(localStorage.getItem(key)).toEqual('noop');
            expect(await storage.get(key)).toBeNull();
        });
    });
});
