import React from 'react';
import ReactDOM from 'react-dom';
import ModelLoader from './model-loader';

it('renders without crashing', () => {
    const div = document.createElement('div');
    const name = 'volume';
    const properties = {};
    const loader = {name, properties};
    ReactDOM.render((
        <ModelLoader
            value={loader}
        />
    ), div);
});
