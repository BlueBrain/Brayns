import React from 'react';

import {createShallow} from '@material-ui/core/test-utils';
import ImageStream from './image-stream';

it('renders without crashing', () => {
    const shallow = createShallow();
    shallow((
        <ImageStream />
    ));
});
