import {noop} from 'lodash';
import React from 'react';
import ReactDOM from 'react-dom';
import AppInfo from './app-info';

it('renders without crashing', () => {
    const div = document.createElement('div');
    ReactDOM.render((
        <AppInfo
            open={false}
            onClose={noop}
        />
    ), div);
});
