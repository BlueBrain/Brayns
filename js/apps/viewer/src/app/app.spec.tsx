import React from 'react';

import {createShallow} from '@material-ui/core/test-utils';
import App from './app';

it('renders without crashing', () => {
    const shallow = createShallow();
    shallow((
        <App />
    ));
});
