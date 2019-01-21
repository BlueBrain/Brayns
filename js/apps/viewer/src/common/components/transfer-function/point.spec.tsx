import React from 'react';

import {createMount} from '@material-ui/core/test-utils';
import Point from './point';

it('renders without crashing', () => {
    const mount = createMount();
    const component = mount(
        <svg>
            <Point cx={0} cy={0} r={2} />
        </svg>
    );
    component.unmount();
    mount.cleanUp();
});

it('renders 2 <circle> nodes', () => {
    const mount = createMount();
    const component = mount(
        <svg>
            <Point
                cx={0}
                cy={0}
                r={2}
                color="#fff"
            />
        </svg>
    );

    const circles = component.find('circle');
    expect(circles).toHaveLength(2);

    expect(circles.at(0)
        .prop('fillOpacity')).toBe(0);
    expect(circles.at(0)
        .prop('r')).toBe(4);
    expect(circles.at(1)
        .prop('fill')).toBe('#fff');
    expect(circles.at(1)
        .prop('r')).toBe(2);

    component.unmount();
    mount.cleanUp();
});

it('renders the <circle> nodes with coords from {cx, cy} props', () => {
    const mount = createMount();
    const component = mount(
        <svg>
            <Point cx={20} cy={50} r={2} />
        </svg>
    );

    const circles = component.find('circle');

    circles.forEach(circle => {
        expect(circle.prop('cx')).toBe(20);
        expect(circle.prop('cy')).toBe(50);
    });

    component.unmount();
    mount.cleanUp();
});

it('prevents default on mousedown on the <g> node', () => {
    const mount = createMount();
    const component = mount(
        <svg>
            <Point cx={0.2} cy={0.5} r={2} />
        </svg>
    );

    const g = component.find('g');

    const preventDefaultSpy = jest.fn();
    g.simulate('mousedown', {
        preventDefault: preventDefaultSpy
    });

    expect(preventDefaultSpy).toHaveBeenCalled();

    component.unmount();
    mount.cleanUp();
});

it('binds mousemove/mouseup to the document when mousedown on the <g> node', () => {
    const mount = createMount();
    const component = mount(
        <svg>
            <Point cx={20} cy={50} r={2} />
        </svg>
    );

    const g = component.find('g');

    const addEventListenerSpy = spyOn(document, 'addEventListener');

    g.simulate('mousedown');

    expect(addEventListenerSpy).toHaveBeenCalledTimes(2);

    const [mousemove, mouseup] = addEventListenerSpy.calls.all();

    expect(mousemove.args[0]).toBe('mousemove');
    expect(mouseup.args[0]).toBe('mouseup');
    expect(mouseup.args[2]).toEqual({
        once: true
    });

    component.unmount();
    mount.cleanUp();
});

it('calls {onChange} with evt on mousemove', () => {
    const mount = createMount();
    const onChangeSpy = jest.fn();
    const component = mount(
        <svg>
            <Point
                cx={20}
                cy={50}
                r={2}
                onChange={onChangeSpy}
            />
        </svg>
    );

    const g = component.find('g');

    g.simulate('mousedown');

    const mouseMoveEvt = new MouseEvent('mousemove');
    const preventDefaultSpy = spyOn(mouseMoveEvt, 'preventDefault');
    const stopPropagationSpy = spyOn(mouseMoveEvt, 'stopPropagation');

    document.dispatchEvent(mouseMoveEvt);

    expect(preventDefaultSpy).toHaveBeenCalled();
    expect(stopPropagationSpy).toHaveBeenCalled();

    expect(onChangeSpy).toHaveBeenCalledWith(mouseMoveEvt);

    component.unmount();
    mount.cleanUp();
});

it('unbinds mousemove from the document when mouseup occurs', () => {
    const mount = createMount();
    const component = mount(
        <svg>
            <Point cx={20} cy={50} r={2} />
        </svg>
    );

    const g = component.find('g');

    const addEventListenerSpy = spyOn(document, 'addEventListener').and.callThrough();
    g.simulate('mousedown');

    const mouseUpEvt = new MouseEvent('mouseup');
    const removeEventListenerSpy = spyOn(document, 'removeEventListener');

    document.dispatchEvent(mouseUpEvt);
    document.dispatchEvent(mouseUpEvt);

    const [, fn] = addEventListenerSpy.calls.first().args;

    expect(removeEventListenerSpy).toHaveBeenCalledWith('mousemove', fn);

    component.unmount();
    mount.cleanUp();
});

it('calls {onRemove} if mousedown on <g> occured with SHIFT key pressed', () => {
    const mount = createMount();

    const onRemoveSpy = jest.fn();
    const component = mount(
        <svg>
            <Point
                cx={20}
                cy={50}
                r={2}
                onRemove={onRemoveSpy}
            />
        </svg>
    );

    const g = component.find('g');

    const addEventListenerSpy = spyOn(document, 'addEventListener');

    const stopPropagationSpy = jest.fn();
    g.simulate('mousedown', {
        stopPropagation: stopPropagationSpy,
        shiftKey: true
    });

    expect(addEventListenerSpy).not.toHaveBeenCalled();
    expect(stopPropagationSpy).toHaveBeenCalled();
    expect(onRemoveSpy).toHaveBeenCalled();

    component.unmount();
    mount.cleanUp();
});
