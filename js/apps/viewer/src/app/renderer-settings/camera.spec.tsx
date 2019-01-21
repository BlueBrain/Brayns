import {shallow} from 'enzyme';
// import {createMount, createShallow} from '@material-ui/core/test-utils';
import React from 'react';
import CameraSettings from './camera';

it('renders without crashing', () => {
    const comp = shallow(<CameraSettings />);
    comp.unmount();
});
