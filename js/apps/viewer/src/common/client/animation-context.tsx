import React, {
    Component,
    createContext,
    ComponentType,
    PureComponent
} from 'react';

import {
    AnimationParameters,
    GET_ANIMATION_PARAMS,
    SET_ANIMATION_PARAMS
} from 'brayns';
import {isNumber} from 'lodash';
import {Subscription} from 'rxjs';
import {mergeMap, map} from 'rxjs/operators';

import brayns, {onReady} from './client';
import {dispatchNotification} from '../events';


export const AnimationContext = createContext<AnimationContextValue>({
    hasAnimation: false,
    animationParams: {},
    isAnimating: false,
    delta: 1,
    onFrameChange: () => Promise.resolve(),
    onFrameNext: () => Promise.resolve(),
    onFramePrev: () => Promise.resolve(),
    onToggle: () => Promise.resolve(),
    onPlay: () => Promise.resolve(),
    onStop: () => Promise.resolve(),
    onDeltaChange: (delta: number) => Promise.resolve()
});

export class AnimationProvider extends Component<{}, State> {
    state: State = {
        animationParams: {},
        isAnimating: false,
        delta: 1
    };

    private subs: Subscription[] = [];

    play = () => this.changeAnimationParams({delta: this.state.delta});

    stop = () => this.changeAnimationParams({delta: 0})

    toggle = async () => {
        const isAnimating = !this.state.isAnimating;
        await this.changeAnimationParams({
            delta: isAnimating ? this.state.delta : 0
        });
    }

    onDeltaChange = async (delta: number) => {
        this.setState({delta});
        if (this.state.isAnimating) {
           await this.changeAnimationParams({delta});
        }
    }

    prevFrame = async () => {
        const {
            animationParams = {}
        } = this.state;
        const {current} = animationParams;
        if (isNumber(current)) {
            const frame = current - 1;
            if (frame >= 0) {
                await this.changeFrame(frame);
            }
        }
    }

    nextFrame = async () => {
        const {
            animationParams = {}
        } = this.state;
        const {current, end} = animationParams;
        if (isNumber(current) && isNumber(end)) {
            const frame = current + 1;
            if (frame <= end) {
                await this.changeFrame(frame);
            }
        }
    }

    changeFrame = (current: number) => this.changeAnimationParams({current})

    changeAnimationParams = async (params: Partial<AnimationParameters>) => {
        try {
            await brayns.request(SET_ANIMATION_PARAMS, params);
            this.setState(state => {
                const animationParams = {
                    ...state.animationParams as Partial<AnimationParameters>,
                    ...params
                };
                return {
                    animationParams,
                    isAnimating: isNumber(animationParams.delta) && animationParams.delta > 0
                };
            });
        } catch (err) {
            dispatchNotification(err);
        }
    }

    componentDidMount() {
        this.subs.push(...[
            onReady().pipe(
                mergeMap(() => brayns.request(GET_ANIMATION_PARAMS)),
                map(toState))
                .subscribe(state => this.setState(state)),
            brayns.observe(SET_ANIMATION_PARAMS)
                .pipe(map(toState))
                .subscribe(state => this.setState(state))
        ]);
    }

    componentWillUnmount() {
        while (this.subs.length) {
            const sub = this.subs.pop();
            sub!.unsubscribe();
        }
    }

    render() {
        const {children} = this.props;
        const {
            hasAnimation,
            animationParams,
            frameCount,
            isAnimating,
            delta
        } = this.state;
        const context: AnimationContextValue = {
            hasAnimation,
            animationParams,
            frameCount,
            isAnimating,
            delta,
            onFrameChange: this.changeFrame,
            onFrameNext: this.nextFrame,
            onFramePrev: this.prevFrame,
            onPlay: this.play,
            onStop: this.stop,
            onToggle: this.toggle,
            onDeltaChange: this.onDeltaChange
        };
        return (
            <AnimationContext.Provider value={context}>
                {children}
            </AnimationContext.Provider>
        )
    }
}


export function withAnimation<P>(Component: ComponentType<P & WithAnimation>): ComponentType<P & WithAnimation> {
    return class extends PureComponent<P & WithAnimation> {
        render() {
            const childFn = (context: WithAnimation) => (
                <Component
                    {...this.props}
                    {...context}
                />
            );
            return (
                <AnimationContext.Consumer>
                    {childFn}
                </AnimationContext.Consumer>
            );
        }
    };
}


function toState(animationParams: AnimationParameters): State {
    const frameCount = getFramesCount(animationParams);
    return {
        animationParams,
        frameCount,
        isAnimating: animationParams.delta > 0,
        hasAnimation: frameCount > 0
    };
}

function getFramesCount(params: Partial<AnimationParameters>): number {
    if (isNumber(params.start) && isNumber(params.end)) {
        return params.end - params.start;
    }
    return 0;
}


interface State {
    hasAnimation?: boolean;
    animationParams?: Partial<AnimationParameters>;
    frameCount?: number;
    isAnimating?: boolean;
    delta?: number;
}

export type WithAnimation = Partial<AnimationContextValue>;

export interface AnimationContextValue extends State {
    onFrameChange(frame: number): Promise<void>;
    onFramePrev(): Promise<void>;
    onFrameNext(): Promise<void>;
    onToggle(): Promise<void>;
    onPlay(delta?: number): Promise<void>;
    onStop(): Promise<void>;
    onDeltaChange(delta: number): Promise<void>;
}
