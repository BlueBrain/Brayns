import React from 'react';

import {shallow} from 'enzyme';
import RequestNotifications from './request-notifications';

it('renders without crashing', () => {
    const component = shallow(
        <RequestNotifications />
    );
    component.unmount();
});
