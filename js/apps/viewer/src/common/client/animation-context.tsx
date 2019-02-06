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
        animationParams: {}
    };

    private subs: Subscription[] = [];

    play = () => this.changeAnimationParams({playing: true});

    stop = () => this.changeAnimationParams({playing: false});

    toggle = async () => {
        const {playing}: Partial<AnimationParameters> = this.state.animationParams || {};
        await this.changeAnimationParams({
            playing: !playing
        });
    }

    onDeltaChange = (delta: number) => this.changeAnimationParams({delta});

    prevFrame = async () => {
        const {
            animationParams = {}
        } = this.state;
        const {current, delta} = animationParams;
        if (isNumber(current) && isNumber(delta)) {
            const frame = current - delta;
            if (frame >= 0) {
                await this.changeFrame(frame);
            }
        }
    }

    nextFrame = async () => {
        const {
            animationParams = {}
        } = this.state;
        const {current, delta, frameCount} = animationParams;
        if (isNumber(current) && isNumber(delta) && isNumber(frameCount)) {
            const frame = current + delta;
            if (frame < frameCount) {
                await this.changeFrame(frame);
            }
        }
    }

    changeFrame = (current: number) => this.changeAnimationParams({current});

    changeAnimationParams = async (params: Partial<AnimationParameters>) => {
        try {
            await brayns.request(SET_ANIMATION_PARAMS, params);
            this.setState(state => ({
                animationParams: {
                    ...state.animationParams as Partial<AnimationParameters>,
                    ...params
                }
            }));
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
            animationParams
        } = this.state;
        const context: AnimationContextValue = {
            hasAnimation,
            animationParams,
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
    return {
        animationParams,
        hasAnimation: animationParams.frameCount > 0
    };
}

interface State {
    hasAnimation?: boolean;
    animationParams?: Partial<AnimationParameters>;
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
