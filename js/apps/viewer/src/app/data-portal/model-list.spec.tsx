import React from 'react';
import ReactDOM from 'react-dom';
import ModelList from './model-list';

it('renders without crashing', () => {
    const div = document.createElement('div');
    ReactDOM.render((
        <ModelList
            models={[]}
        />
    ), div);
});
