import React from 'react';

import {createShallow} from '@material-ui/core/test-utils';
import RendererSettings from './renderer-settings';

it('renders without crashing', () => {
    const shallow = createShallow();
    const comp = shallow((
        <RendererSettings />
    ));
    comp.unmount();
});
