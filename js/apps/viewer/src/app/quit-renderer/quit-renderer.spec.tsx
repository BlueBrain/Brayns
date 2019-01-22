import React from 'react';
import ReactDOM from 'react-dom';
import QuitRenderer from './quit-renderer';

it('renders without crashing', () => {
    const div = document.createElement('div');
    ReactDOM.render((
        <QuitRenderer />
    ), div);
});
