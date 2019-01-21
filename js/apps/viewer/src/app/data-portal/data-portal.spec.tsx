import React from 'react';

import {shallow} from 'enzyme';
import DataPortal from './data-portal';

it('renders without crashing', () => {
    const component = shallow(
        <DataPortal />
    );
    component.unmount();
});
