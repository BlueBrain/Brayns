import React from 'react';

import {createShallow} from '@material-ui/core/test-utils';
import SceneModels from './scene';

it('renders without crashing', () => {
    const shallow = createShallow();
    const component = shallow(<SceneModels />);
    component.unmount();
});
