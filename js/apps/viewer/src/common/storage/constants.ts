import LocalStorage from './local-storage';

const NS_KEY = 'brayns';
const storage = LocalStorage.create(NS_KEY);

export {
    storage as default
};
