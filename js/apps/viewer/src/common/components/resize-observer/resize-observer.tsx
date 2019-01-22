import React, {
    ComponentType,
    createRef,
    PureComponent
} from 'react';
import {Observable, Subscription} from 'rxjs';


const DEFAULT_RECT: ClientRect = {
    bottom: 0,
    height: 0,
    left: 0,
    right: 0,
    top: 0,
    width: 0
};

// https://reactjs.org/docs/higher-order-components.html
// https://americanexpress.io/faccs-are-an-antipattern
export function withResizeObserver<P>(Component: ComponentType<P & WithRect>): ComponentType<P & WithRect> {
    return class extends PureComponent<P & WithRect, State> {
        state: State = {
            rect: DEFAULT_RECT
        };

        private nodeRef = createRef<HTMLDivElement>();
        private sub?: Subscription;

        componentDidMount() {
            const parent = this.nodeRef.current;

            if (parent) {
                const child = parent.firstElementChild;
                this.sub = createCheckSizeLoop(child!)
                    .subscribe(rect => {
                        this.setState({rect});
                    });
            }
        }

        componentWillUnmount() {
            if (this.sub) {
                this.sub.unsubscribe();
            }
        }

        render() {
            const {rect} = this.state;

            return (
                <div ref={this.nodeRef}>
                    <Component
                        {...this.props}
                        rect={rect}
                    />
                </div>
            );
        }
    };
}

function createCheckSizeLoop(node: Element) {
    let prevRect = DEFAULT_RECT;
    let rAFId: number;

    return new Observable<ClientRect>(observer => {
        checkSize();

        function checkSize() {
            rAFId = requestAnimationFrame(checkSize);
            const rect = node.getBoundingClientRect();

            if (!compareRect(rect, prevRect)) {
                prevRect = rect;
                observer.next(rect);
            }
        }

        return () => {
            if (rAFId) {
                cancelAnimationFrame(rAFId);
            }
        };
    });
}

function compareRect(a: ClientRect, b?: ClientRect) {
    if (!b) {
        return false;
    }
    return a.width === b.width
        && a.height === b.height
        && a.top === b.top
        && a.right === b.right
        && a.bottom === b.bottom
        && a.left === b.left;
}

interface State {
    rect: ClientRect;
}

export interface WithRect {
    rect?: ClientRect;
}
