import React from 'react';

import {shallow} from 'enzyme';
import ConnectionStatus from './connection-status';

it('renders without crashing', () => {
    const component = shallow(
        <ConnectionStatus />
    );
    component.unmount();
});
