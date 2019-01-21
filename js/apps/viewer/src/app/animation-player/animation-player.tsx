import React, {PureComponent} from 'react';

import {
    AnimationParameters,
    GET_ANIMATION_PARAMS,
    SET_ANIMATION_PARAMS
} from 'brayns';
import {Subscription} from 'rxjs';
import {mergeMap} from 'rxjs/operators';

import Slide from '@material-ui/core/Slide';
import {
    createMuiTheme,
    createStyles,
    MuiThemeProvider,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import Typography from '@material-ui/core/Typography';

import brayns, {isReady, onReady} from '../../common/client';
import {KeyCode} from '../../common/constants';
import {onKeyboardLockChange, onPageVisibilityChange} from '../../common/events';

import Controls from './controls';
import ProgressBar from './progress-bar';
import {
    currentFrame,
    framesCount,
    frameToTimeStr,
    hasAnimation
} from './utils';


const controlsHeight = 15 + 48;

const styles = (theme: Theme) => createStyles({
    container: {
        position: 'absolute',
        display: 'flex',
        bottom: 0,
        width: '100%',
        height: controlsHeight,
        padding: `0 ${theme.spacing.unit * 2}px`,
        flexDirection: 'column',
        alignItems: 'flex-start',
        outline: 'none'
    },
    bg: {
        position: 'absolute',
        width: '100%',
        height: 100,
        bottom: -100,
        left: 0,
        zIndex: -1,
        boxShadow: '0px -40px 100px rgba(0, 0, 0, 0.25)'
    },
    controls: {
        display: 'flex',
        flexDirection: 'row',
        alignItems: 'center',
        width: '100%',
        height: 40
    },
    spacer: {
        flex: 1
    }
});

const theme = createMuiTheme({
    palette: {
        type: 'dark' // Use dark theme for the play/pause buttons
    }
});

const style = withStyles(styles);


export class AnimationPlayer extends PureComponent<Props, State> {
    state: State = {
        animate: false,
        hide: false
    };

    private subs: Subscription[] = [];
    private keyboardEventsListenerAttached = false;
    private keyboardLocked = false;

    playAnimation = () => {
        this.updateAnimationParams({
            delta: 1
        });
        this.setState({
            animate: true,
            suspended: false
        });
    }

    stopAnimation = (suspended?: boolean) => {
        this.updateAnimationParams({
            delta: 0
        });
        this.setState({
            suspended,
            animate: false
        });
    }

    togglePlay = () => {
        this.setState(prev => {
            const animate = !prev.animate;
            this.updateAnimationParams({
                delta: Number(animate)
            });
            return {animate};
        });
    }

    jumpToFrame = (params: Partial<AnimationParameters>) => {
        this.updateAnimationParams({
            current: params.current
        });
    }
    prevFrame = () => {
        const {animationParams = {}} = this.state;
        const current = currentFrame(animationParams);
        const prev = current - 1;
        if (prev >= 0) {
            this.updateAnimationParams({
                current: prev
            });
        }
    }
    nextFrame = () => {
        const {animationParams = {}} = this.state;
        const current = currentFrame(animationParams);
        const end = animationParams.end || 0;
        const next = current + 1;
        if (next <= end) {
            this.updateAnimationParams({
                current: next
            });
        }
    }

    toggleKeyboardEventsListener = (params?: AnimationParameters) => {
        const canAnimate = hasAnimation(params);
        if (canAnimate && !this.keyboardEventsListenerAttached) {
            this.attachKeyboardEventsListener();
        } else if (!canAnimate && this.keyboardEventsListenerAttached) {
            this.detachKeyboardEventsListener();
        }
    }

    attachKeyboardEventsListener = () => {
        window.addEventListener('keydown', this.handleKeydown, false);
        this.keyboardEventsListenerAttached = true;
    }

    detachKeyboardEventsListener = () => {
        window.removeEventListener('keydown', this.handleKeydown);
        this.keyboardEventsListenerAttached = false;
    }

    handleKeydown = (evt: KeyboardEvent) => {
        const {animate} = this.state;

        if (this.keyboardLocked) {
            return;
        }

        switch (evt.keyCode) {
            case KeyCode.Space:
                stopBubbling();
                if (animate) {
                    this.stopAnimation();
                } else {
                    this.playAnimation();
                }
                break;
            case KeyCode.P:
                if (!animate && evt.shiftKey) {
                    stopBubbling();
                    this.prevFrame();
                }
                break;
            case KeyCode.N:
                if (!animate && evt.shiftKey) {
                    stopBubbling();
                    this.nextFrame();
                }
                break;
            default:
                break;
        }

        function stopBubbling() {
            evt.preventDefault();
            evt.stopPropagation();
        }
    }

    updateAnimationParams = async (params: Partial<AnimationParameters>) => {
        const ready = await isReady();
        if (!ready) {
            return;
        }
        this.setState(state => ({
            animationParams: {
                ...state.animationParams,
                ...params
            }
        }), () => {
            brayns.notify(SET_ANIMATION_PARAMS, params);
        });
    }

    componentDidMount() {
        this.subs.push(...[
            // Listen to animation params change
            brayns.observe(SET_ANIMATION_PARAMS)
                .subscribe(params => {
                    this.toggleKeyboardEventsListener(params);

                    this.setState({
                        animationParams: params,
                        animate: !!params.delta
                    });
                }),
            // Get current request animation params
            onReady().pipe(mergeMap(() => brayns.request(GET_ANIMATION_PARAMS)))
                .subscribe(params => {
                    this.toggleKeyboardEventsListener(params);

                    this.setState({
                        animationParams: params,
                        animate: !!params.delta
                    });
                }),
            onPageVisibilityChange()
                .subscribe(hidden => {
                    if (hidden && this.state.animate) {
                        this.stopAnimation(true);
                    } else if (this.state.suspended) {
                        this.playAnimation();
                    }
                }),
            onKeyboardLockChange()
                .subscribe(lock => {
                    this.keyboardLocked = lock;
                })
        ]);
    }

    componentWillUnmount() {
        this.detachKeyboardEventsListener();

        for (const sub of this.subs) {
            sub.unsubscribe();
        }
    }

    render() {
        const {classes, disabled} = this.props;
        const {
            animationParams = {},
            animate,
            hide
        } = this.state;
        const hasAnim = hasAnimation(animationParams);
        const showPlayer = hasAnim && !hide && !disabled;
        const disableBtns = !hasAnim || disabled;

        const total = framesCount(animationParams);
        const totalMs = frameToTimeStr(total, animationParams.dt);
        const currentMs = frameToTimeStr(animationParams.current, animationParams.dt);

        return (
            <Slide direction="up" in={showPlayer}>
                <div className={classes.container}>
                    <div className={classes.bg} />
                    <ProgressBar
                        animationParams={animationParams}
                        onFrameChange={this.jumpToFrame}
                    />
                    <MuiThemeProvider theme={theme}>
                        <div className={classes.controls}>
                            <Controls
                                play={!!animate}
                                onPlayToggle={this.togglePlay}
                                onPrev={this.prevFrame}
                                onNext={this.nextFrame}
                                disablePrev={animationParams.current === 0}
                                disableNext={animationParams.current === animationParams.end}
                                disabled={disableBtns}
                            />
                            <Typography variant="caption">
                                {currentMs} / {totalMs} {animationParams.unit}
                            </Typography>
                        </div>
                    </MuiThemeProvider>
                </div>
            </Slide>

        );
    }
}

export default style(AnimationPlayer);


interface Props extends WithStyles<typeof styles> {
    disabled?: boolean;
}

interface State {
    animationParams?: Partial<AnimationParameters>;
    animate?: boolean;
    suspended?: boolean;
    hide?: boolean;
}
