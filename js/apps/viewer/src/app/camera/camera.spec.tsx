import React from 'react';

import {createShallow} from '@material-ui/core/test-utils';
import Camera from './camera';

it('renders without crashing', () => {
    const shallow = createShallow();
    shallow((
        <Camera />
    ));
});
