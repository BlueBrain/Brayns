import {noop} from 'lodash';
import React from 'react';
import ReactDOM from 'react-dom';
import Shortcuts from './shortcuts';

it('renders without crashing', () => {
    const div = document.createElement('div');
    ReactDOM.render((
        <Shortcuts
            onClose={noop}
        />
    ), div);
});
