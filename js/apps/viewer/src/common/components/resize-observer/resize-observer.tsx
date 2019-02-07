import React, {
    ComponentType,
    createRef,
    PureComponent,
    RefObject
} from 'react';

import {isNumber} from 'lodash';
import {BehaviorSubject, Subscription} from 'rxjs';


const DEFAULT_RECT: ClientRect = {
    bottom: 0,
    height: 0,
    left: 0,
    right: 0,
    top: 0,
    width: 0
};


// TODO: Update tests!
// TODO: Switch to hooks
// https://reactjs.org/docs/higher-order-components.html
// https://americanexpress.io/faccs-are-an-antipattern
export function withResizeObserver<P>(Component: ComponentType<P & WithRect>): ComponentType<P & WithRect> {
    return class extends PureComponent<P & WithRect, State> {
        state: State = {
            rect: {...DEFAULT_RECT}
        };

        private rectRef = createRef<Element>();
        private rectSubject?: RectSubject;
        private sub?: Subscription;

        componentDidMount() {
            const node = this.rectRef.current;
            if (node) {
                this.rectSubject = queueNodeForSizeCheck(node);
                this.sub = this.rectSubject.subscribe(rect => {
                    this.setState({rect});
                });
            }
        }

        componentWillUnmount() {
            const node = this.rectRef.current;
            if (node) {
                stopLoop(node);
            }

            if (this.sub) {
                this.sub.unsubscribe();
            }
        }

        render() {
            const {rect} = this.state;

            return (
                <Component
                    rectRef={this.rectRef}
                    rectChanges={this.rectSubject}
                    rect={rect}
                    {...this.props}
                />
            );
        }
    }
}


let rAFId: number | null;
let isLoopInProgress = false;

const nodes = new Map<Element, RectSubject>();

// https://caniuse.com/#feat=resizeobserver
const ResizeObserver: ResizeObserverCtor = (window as any).ResizeObserver;
const ro: ResizeObserver | null = ResizeObserver ? new ResizeObserver(entries => {
    for (const entry of entries) {
        const subject = nodes.get(entry.target);
        if (subject) {
            subject.next(entry.contentRect);
        }
    }
}) : null;


function queueNodeForSizeCheck(node: Element) {
    const subject = new BehaviorSubject<ClientRect>({...DEFAULT_RECT});
    if (!nodes.has(node)) {
        nodes.set(node, subject);
    }

    if (ro) {
        ro.observe(node);
    } else if (!isLoopInProgress) {
        startLoop();
    }

    return subject;
}

function startLoop() {
    rAFId = requestAnimationFrame(startLoop);

    for (const [node, subject] of nodes.entries()) {
        const rect = node.getBoundingClientRect();
        if (!compareRect(rect, subject.value)) {
            subject.next(rect);
        }
    }
}

function stopLoop(node: Element) {
    nodes.delete(node);
    const noNodesLeft = nodes.size === 0;

    if (ro) {
        ro.unobserve(node);
        if (noNodesLeft) {
            ro.disconnect();
        }
    } else if (noNodesLeft && isNumber(rAFId)) {
        cancelAnimationFrame(rAFId);
        rAFId = null;
    }
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


export type Rect = DOMRect | ClientRect;

export interface WithRect {
    rect?: Rect;
    rectRef?: RefObject<any>;
    rectChanges?: RectSubject;
}

type RectSubject = BehaviorSubject<Rect>;


// TODO: Eventually these will be implemented by the TS dom lib,
// at that point we have to remove these.
// https://drafts.csswg.org/resize-observer-1/#resize-observer-interface
interface ResizeObserverCtor {
    new (cb: (entries: ResizeObserverEntry[]) => void): ResizeObserver;
}

interface ResizeObserver {
    observe(target: Element, options?: ResizeObserverOptions): void;
    unobserve(target: Element): void;
    disconnect(): void;
}

interface ResizeObserverOptions {
    box: 'border-box'
        | 'content-box'
        | 'scroll-box'
        | 'device-pixel-border-box';
};

interface ResizeObserverEntry {
    contentRect: DOMRect;
    target: Element;
}
