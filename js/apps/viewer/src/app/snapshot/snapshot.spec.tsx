import React from 'react';
import ReactDOM from 'react-dom';
import Snapshot from './snapshot';

it('renders without crashing', () => {
    const div = document.createElement('div');
    ReactDOM.render((
        <Snapshot />
    ), div);
});
