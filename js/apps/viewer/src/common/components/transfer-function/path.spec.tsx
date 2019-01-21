import React from 'react';

import {createMount} from '@material-ui/core/test-utils';
import Path from './path';
import {Provider} from './provider';
import {
    comparePathD,
    createPoint,
    createRect
} from './testing';


it('renders without crashing', () => {
    const mount = createMount();
    const canvas = createRect();
    const component = mount(
        <svg>
            <Provider value={{canvas}}>
                <Path points={[]} />
            </Provider>
        </svg>
    );
    component.unmount();
    mount.cleanUp();
});

it('renders a <path> node', () => {
    const mount = createMount();
    const canvas = createRect();
    const component = mount(
        <svg>
            <Provider value={{canvas}}>
                <Path points={[]} />
            </Provider>
        </svg>
    );

    const path = component.find('path');
    expect(path).toHaveLength(1);

    component.unmount();
    mount.cleanUp();
});

it('renders a <path> with empty {d} if provided data does not have enough points', () => {
    const mount = createMount();
    const canvas = createRect();

    const points = [createPoint(0, 0)];
    const component = mount(
        <svg>
            <Provider value={{canvas}}>
                <Path points={points} />
            </Provider>
        </svg>
    );

    const path = component.find('path');
    expect(path.prop('d')).toEqual('');
    expect(path.prop('fillOpacity')).toEqual(0);

    component.unmount();
    mount.cleanUp();
});

it('renders a <path> based on provided points', () => {
    const mount = createMount();
    const canvas = createRect(1, 1);

    const points = [
        createPoint(0, 0),
        createPoint(0.5, 0.5)
    ];
    const component = mount(
        <svg>
            <Provider value={{canvas}}>
                <Path points={points} />
            </Provider>
        </svg>
    );

    const path = component.find('path');
    expect(comparePathD(path.prop('d'), 'M 0,0 L 0.5, 0.5')).toBe(true);

    component.unmount();
    mount.cleanUp();
});

it('renders a <path> with points scaled to canvas rect', () => {
    const mount = createMount();
    const canvas = createRect(100, 100);

    const points = [
        createPoint(0, 0),
        createPoint(0.5, 0.5)
    ];
    const component = mount(
        <svg>
            <Provider value={{canvas}}>
                <Path points={points} />
            </Provider>
        </svg>
    );

    const path = component.find('path');
    expect(comparePathD(path.prop('d'), 'M 0,0 L 50, 50')).toBe(true);

    component.unmount();
    mount.cleanUp();
});
