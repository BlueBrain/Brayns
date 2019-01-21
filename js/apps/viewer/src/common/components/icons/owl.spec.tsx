import React from 'react';

import {createShallow} from '@material-ui/core/test-utils';
import {OwlIcon} from './owl';

it('renders without crashing', () => {
    const shallow = createShallow({dive: true});
    const icon = shallow((<OwlIcon />));
    icon.unmount();
});
