import React, {
    Component,
    ComponentType,
    createContext,
    PureComponent
} from 'react';

import {
    AnimationParameters,
    GET_ANIMATION_PARAMS,
    SET_ANIMATION_PARAMS
} from 'brayns';
import {isNumber} from 'lodash';
import {Subscription} from 'rxjs';
import {map, mergeMap} from 'rxjs/operators';

import {dispatchNotification} from '../events';
import brayns, {onReady} from './client';


export const AnimationContext = createContext<AnimationContextValue>({
    hasAnimation: false,
    animationParams: {},
    isAnimating: false,
    onFrameChange: () => Promise.resolve(),
    onFrameNext: () => Promise.resolve(),
    onFramePrev: () => Promise.resolve(),
    onToggle: () => Promise.resolve(),
    onPlay: () => Promise.resolve(),
    onStop: () => Promise.resolve()
});

export class AnimationProvider extends Component<{}, State> {
    state: State = {
        animationParams: {},
        isAnimating: false
    };

    private subs: Subscription[] = [];

    play = (delta: number = 1) => this.changeAnimationParams({delta});

    stop = () => this.changeAnimationParams({delta: 0});

    toggle = async () => {
        const isAnimating = !this.state.isAnimating;
        await this.changeAnimationParams({
            delta: Number(isAnimating)
        });
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

    changeFrame = (current: number) => this.changeAnimationParams({current});

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
            isAnimating
        } = this.state;
        const context: AnimationContextValue = {
            hasAnimation,
            animationParams,
            frameCount,
            isAnimating,
            onFrameChange: this.changeFrame,
            onFrameNext: this.nextFrame,
            onFramePrev: this.prevFrame,
            onPlay: this.play,
            onStop: this.stop,
            onToggle: this.toggle
        };
        return (
            <AnimationContext.Provider value={context}>
                {children}
            </AnimationContext.Provider>
        );
    }
}


// tslint:disable: max-classes-per-file
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
}

export type WithAnimation = Partial<AnimationContextValue>;

export interface AnimationContextValue extends State {
    onFrameChange(frame: number): Promise<void>;
    onFramePrev(): Promise<void>;
    onFrameNext(): Promise<void>;
    onToggle(): Promise<void>;
    onPlay(delta?: number): Promise<void>;
    onStop(): Promise<void>;
}
