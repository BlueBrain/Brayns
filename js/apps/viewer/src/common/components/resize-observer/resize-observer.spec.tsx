import React, {PureComponent} from 'react';

import {createMount} from '@material-ui/core/test-utils';
import {WithRect, withResizeObserver} from './resize-observer';


it('renders without crashing', () => {
    const mount = createMount();
    const ResMockComponent = withResizeObserver<Props>(MockComponent);
    const component = mount(<ResMockComponent />);
    component.unmount();
    mount.cleanUp();
});

it('has all {rect} prop props set to 0 on first render', () => {
    const mount = createMount();
    const ResMockComponent = withResizeObserver<Props>(MockComponent);
    const component = mount(<ResMockComponent />);

    const mockComponent = component.find(MockComponent);

    const rect = createRect();
    expect(mockComponent.prop('rect')).toEqual(rect);

    component.unmount();
    mount.cleanUp();
});

it.skip('renders only when {rect} prop changes', () => {
    const mount = createMount();
    const ResMockComponent = withResizeObserver<Props>(MockComponent);

    const renderSpy = jest.fn();
    const component = mount(<ResMockComponent onRender={renderSpy} />);

    expect(renderSpy).toHaveBeenCalledTimes(1);

    component.unmount();
    mount.cleanUp();
});

it.skip('gets {rect} prop updates if the size of the component changes', done => {
    const mount = createMount();
    const ResMockComponent = withResizeObserver<Props>(MockComponent);

    let count = 0;
    const defaultRect = createRect();
    const bBoxGetter = () => {
        const size = 10 * count;
        const rect = createRect(size, size);

        count++;

        return count < 10
            ? rect
            : defaultRect;
    };

    const renderSpy = jest.fn();

    const component = mount(
        <ResMockComponent
            bBoxGetter={bBoxGetter}
            onRender={renderSpy}
        />
    );

    setTimeout(() => {
        expect(renderSpy).toHaveBeenCalledTimes(10);

        let count = 0;
        for (const call of renderSpy.mock.calls) {
            const [arg] = call;

            const size = 10 * (count < 9 ? count : 0);
            const rect = createRect(size, size);

            expect(arg).toEqual(rect);

            count++;
        }

        component.unmount();
        mount.cleanUp();
        done();
    }, 1000);
});

it('stops the size check loop on unmount', done => {
    const mount = createMount();
    const ResMockComponent = withResizeObserver<Props>(MockComponent);

    let count = 0;
    const getterSpy = jest.fn();
    const bBoxGetter = () => {
        getterSpy();
        const size = 10 * count;
        const rect = createRect(size, size);
        count++;
        return rect;
    };

    const component = mount(<ResMockComponent bBoxGetter={bBoxGetter} />);


    setTimeout(() => {
        expect(getterSpy).toHaveBeenCalled();
        component.unmount();

        const count = getterSpy.mock.calls.length;

        setTimeout(() => {
            expect(getterSpy).toHaveBeenCalledTimes(count);
            mount.cleanUp();
            done();
        }, 100);
    }, 100);
});


class MockComponent extends PureComponent<Props> {
    componentDidMount() {
        const rectRef = this.props.rectRef;
        if (rectRef && rectRef.current) {
            const {bBoxGetter} = this.props;
            if (bBoxGetter) {
                jest.spyOn(rectRef.current, 'getBoundingClientRect').mockImplementation(bBoxGetter);
            }
        }
    }

    render() {
        const {onRender, rect, rectRef} = this.props;
        if (onRender) {
            onRender(rect);
        }

        return (
            <div ref={rectRef} />
        );
    }
}

interface Props extends WithRect {
    onRender?(rect?: ClientRect): void;
    bBoxGetter?(): ClientRect;
}

function createRect(width: number = 0, height: number = 0): ClientRect {
    return {
        width,
        height,
        bottom: 0,
        left: 0,
        right: 0,
        top: 0
    };
}
