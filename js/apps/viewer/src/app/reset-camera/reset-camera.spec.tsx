import React from 'react';

import {shallow} from 'enzyme';
import ResetCamera from './reset-camera';

it('renders without crashing', () => {
    const component = shallow(
        <ResetCamera />
    );
    component.unmount();
});
