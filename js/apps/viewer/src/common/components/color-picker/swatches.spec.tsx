import React from 'react';
import ReactDOM from 'react-dom';

import {createMount, getClasses} from '@material-ui/core/test-utils';

import {MATERIAL_PALETTES} from './constants';
import Swatches from './swatches';


it('renders without crashing', () => {
    const div = document.createElement('div');
    ReactDOM.render((
        <Swatches />
    ), div);
});

it('renders a color bubble for each Material palette with the primary color as background color', () => {
    const mount = createMount();

    const component = (<Swatches />);
    const classes = getClasses(component);
    const swatches = mount(component);

    const bubbles = swatches.find(`.${classes.bubble}`);

    expect(bubbles).toHaveLength(MATERIAL_PALETTES.length);
    expect(bubbles.map(c => c.prop('style'))).toEqual(MATERIAL_PALETTES.map(color => ({
        backgroundColor: color['500']
    })));

    swatches.unmount();
    mount.cleanUp();
});

it('sets a color bubble as selected if it has the same color as the one provided by {value} prop', () => {
    const mount = createMount();

    const color = MATERIAL_PALETTES[0]['500'];
    const component = (<Swatches value={color} />);
    const classes = getClasses(component);
    const swatches = mount(component);

    const bubble = swatches.find(`.${classes.bubble}`)
        .find(`.${classes.bubbleShift}`);

    expect(bubble).toHaveLength(1);

    expect(bubble.find(`.${classes.bubbleInner}`)
        .hasClass(classes.bubbleInnerShift)).toBe(true);

    swatches.unmount();
    mount.cleanUp();
});

it('sets a color bubble as focused if it\'s being hovered', () => {
    const mount = createMount();

    const component = (<Swatches />);
    const classes = getClasses(component);
    const swatches = mount(component);

    const bubbles = swatches.find(`.${classes.bubble}`);

    bubbles.at(0)
        .simulate('mouseenter');

    const bubble = swatches.find(`.${classes.bubble}`)
        .find(`.${classes.bubbleShift}`);

    expect(bubble).toHaveLength(1);

    swatches.unmount();
    mount.cleanUp();
});

it('unsets a color bubble as focused on unhovered', () => {
    const mount = createMount();

    const component = (<Swatches />);
    const classes = getClasses(component);
    const swatches = mount(component);

    const bubbles = swatches.find(`.${classes.bubble}`);

    bubbles.at(0)
        .simulate('mouseenter')
        .simulate('mouseleave');

    const bubble = swatches.find(`.${classes.bubble}`)
        .find(`.${classes.bubbleShift}`);

    expect(bubble).toHaveLength(0);

    swatches.unmount();
    mount.cleanUp();
});

it('runs {onChange} when a color bubble is clicked', () => {
    const mount = createMount();

    const onChangeSpy = jest.fn();
    const component = (<Swatches onChange={onChangeSpy} />);
    const classes = getClasses(component);
    const swatches = mount(component);

    const bubbles = swatches.find(`.${classes.bubble}`);

    bubbles.at(0)
        .simulate('click');

    expect(onChangeSpy).toHaveBeenCalledTimes(1);

    const [args] = onChangeSpy.mock.calls;
    const [color] = args;

    const style = bubbles.at(0)
        .prop('style');
    expect(color).toBe(style!.backgroundColor);

    swatches.unmount();
    mount.cleanUp();
});
