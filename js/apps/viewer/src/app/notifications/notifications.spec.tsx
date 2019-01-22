import React from 'react';

import {createShallow} from '@material-ui/core/test-utils';
import Notifications from './notifications';

it('renders without crashing', () => {
    const shallow = createShallow();
    const component = shallow(
        <Notifications />
    );
    component.unmount();
});
