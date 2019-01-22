// tslint:disable: no-empty
import React from 'react';
import ReactDOM from 'react-dom';
import ProgressBar from './progress-bar';

it('renders without crashing', () => {
    const div = document.createElement('div');
    const animationParams = {};
    ReactDOM.render((
        <ProgressBar animationParams={animationParams} />
    ), div);
});
