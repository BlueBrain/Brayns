import React from 'react';
import ReactDOM from 'react-dom';
import Statistics from './statistics';

it('renders without crashing', () => {
    const div = document.createElement('div');
    ReactDOM.render(
        <Statistics />
    , div);
});
