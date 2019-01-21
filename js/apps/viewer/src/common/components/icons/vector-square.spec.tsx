import React from 'react';

import {createShallow} from '@material-ui/core/test-utils';
import {VectorSquareIcon} from './vector-square';

it('renders without crashing', () => {
    const shallow = createShallow({dive: true});
    const icon = shallow((<VectorSquareIcon />));
    icon.unmount();
});
