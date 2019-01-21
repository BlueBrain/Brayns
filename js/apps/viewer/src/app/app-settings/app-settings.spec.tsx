import {noop} from 'lodash';
import React from 'react';
import ReactDOM from 'react-dom';
import AppSettings from './app-settings';

it('renders without crashing', () => {
    const div = document.createElement('div');
    ReactDOM.render((
        <AppSettings
            preferences={[]}
            onClose={noop}
        />
    ), div);
});
