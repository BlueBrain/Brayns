import React from 'react';

import {createShallow} from '@material-ui/core/test-utils';
import {GithubIcon} from './github';

it('renders without crashing', () => {
    const shallow = createShallow({dive: true});
    const icon = shallow((<GithubIcon />));
    icon.unmount();
});
