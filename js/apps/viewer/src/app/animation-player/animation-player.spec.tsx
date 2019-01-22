import React from 'react';
import ReactDOM from 'react-dom';
import AnimationPlayer from './animation-player';

it('renders without crashing', () => {
    const div = document.createElement('div');
    ReactDOM.render((
        <AnimationPlayer />
    ), div);
});
