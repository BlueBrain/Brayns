import React, {PureComponent} from 'react';

import {Subscription} from 'rxjs';

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

import {withAnimation, WithAnimation} from '../../common/client';
import {KeyCode} from '../../common/constants';
import {onKeyboardLockChange, onPageVisibilityChange} from '../../common/events';

import Controls from './controls';
import ProgressBar from './progress-bar';
import {frameToTimeStr} from './utils';


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


export class AnimationPlayer extends PureComponent<Props> {
    private subs: Subscription[] = [];
    private keyboardEventsListenerAttached = false;
    private keyboardLocked = false;
    private wasAnimationSuspended = false;

    toggleKeyboardEventsListener = () => {
        const {hasAnimation} = this.props;
        if (hasAnimation && !this.keyboardEventsListenerAttached) {
            this.attachKeyboardEventsListener();
        } else if (!hasAnimation && this.keyboardEventsListenerAttached) {
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
        const {isAnimating} = this.props;

        if (this.keyboardLocked) {
            return;
        }

        switch (evt.keyCode) {
            case KeyCode.Space:
                stopBubbling();
                if (isAnimating) {
                    this.props.onStop!();
                } else {
                    this.props.onPlay!();
                }
                break;
            case KeyCode.P:
                if (!isAnimating && evt.shiftKey) {
                    stopBubbling();
                    this.props.onFramePrev!();
                }
                break;
            case KeyCode.N:
                if (!isAnimating && evt.shiftKey) {
                    stopBubbling();
                    this.props.onFrameNext!();
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

    componentDidMount() {
        this.subs.push(...[
            onPageVisibilityChange()
                .subscribe(async hidden => {
                    const {isAnimating} = this.props;
                    if (hidden && isAnimating) {
                        await this.props.onStop!();
                        this.wasAnimationSuspended = true;
                    } else if (!isAnimating && this.wasAnimationSuspended) {
                        await this.props.onPlay!();
                        this.wasAnimationSuspended = false;
                    }
                }),
            onKeyboardLockChange()
                .subscribe(lock => {
                    this.keyboardLocked = lock;
                })
        ]);
    }

    componentDidUpdate() {
        this.toggleKeyboardEventsListener();
    }

    componentWillUnmount() {
        this.detachKeyboardEventsListener();

        for (const sub of this.subs) {
            sub.unsubscribe();
        }
    }

    render() {
        const {
            classes,
            disabled,
            hasAnimation,
            frameCount = 0,
            animationParams = {},
            isAnimating,
            onToggle,
            onFrameChange,
            onFramePrev,
            onFrameNext
        } = this.props;
        const showPlayer = hasAnimation && !disabled;
        const disableBtns = !hasAnimation || disabled;

        const totalMs = frameToTimeStr(frameCount, animationParams.dt);
        const currentMs = frameToTimeStr(animationParams.current, animationParams.dt);

        return (
            <Slide direction="up" in={showPlayer}>
                <div className={classes.container}>
                    <div className={classes.bg} />
                    <ProgressBar
                        animationParams={animationParams}
                        onFrameChange={onFrameChange!}
                        frameCount={frameCount}
                    />
                    <MuiThemeProvider theme={theme}>
                        <div className={classes.controls}>
                            <Controls
                                play={!!isAnimating}
                                onPlayToggle={onToggle!}
                                onPrev={onFramePrev!}
                                onNext={onFrameNext!}
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

export default style(
    withAnimation(AnimationPlayer)
);


interface Props extends WithStyles<typeof styles>,
    WithAnimation {
    disabled?: boolean;
}
