import React from 'react';

import {createMount, createShallow} from '@material-ui/core/test-utils';

import {NumericField} from '../../common/components';
import ImageQuality from './image-quality';

it('renders without crashing', () => {
    const shallow = createShallow();
    const component = shallow(<ImageQuality />);
    component.unmount();
});

it('renders <NumericField> for every prop', () => {
    const mount = createMount();

    const props = {
        imageStreamFps: 30,
        jpegCompression: 45,
        disabled: true
    };
    const component = mount(<ImageQuality {...props} />);

    const fields = component.find(NumericField);
    expect(fields.length).toBe(2);

    const jpegCompression = fields.at(0);
    expect(jpegCompression.prop('value')).toBe(props.jpegCompression);
    expect(jpegCompression.prop('disabled')).toBe(props.disabled);
    expect(jpegCompression.prop('min')).toBe(1);
    expect(jpegCompression.prop('max')).toBe(100);

    const imageStreamFps = fields.at(1);
    expect(imageStreamFps.prop('value')).toBe(props.imageStreamFps);
    expect(imageStreamFps.prop('disabled')).toBe(props.disabled);
    expect(imageStreamFps.prop('min')).toBe(1);
    expect(imageStreamFps.prop('max')).toBe(60);

    component.unmount();
    mount.cleanUp();
});

it('runs {onChange} if any of the <NumericField> components\' value changes', () => {
    const mount = createMount();

    const onChangeSpy = jest.fn();

    const props = {
        imageStreamFps: 30,
        jpegCompression: 45,
        onChange: onChangeSpy
    };
    const component = mount(<ImageQuality {...props} />);

    const fields = component.find(NumericField);

    const jpegCompression = fields.at(0);
    jpegCompression.props()
        .onChange(10);
    expect(onChangeSpy).toHaveBeenCalledWith({
        jpegCompression: 10
    });

    const imageStreamFps = fields.at(1);
    imageStreamFps.props()
        .onChange(60);
    expect(onChangeSpy).toHaveBeenCalledWith({
        imageStreamFps: 60
    });

    component.unmount();
    mount.cleanUp();
});
