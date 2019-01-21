import {noop} from 'lodash';
import React from 'react';
import ReactDOM from 'react-dom';
import LoadModel from './load-model';

it('renders without crashing', () => {
    const div = document.createElement('div');
    ReactDOM.render((
        <LoadModel
            onPathLoad={noop}
            open={false}
        />
    ), div);
});
