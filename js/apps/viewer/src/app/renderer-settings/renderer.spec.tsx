import {shallow} from 'enzyme';
import React from 'react';
import Renderer from './renderer';

it('renders without crashing', () => {
    const comp = shallow(<Renderer />);
    comp.unmount();
});
