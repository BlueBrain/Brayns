import React, {
    createRef,
    CSSProperties,
    MouseEvent as ReactMouseEvent,
    PureComponent,
    RefObject
} from 'react';

import {AnimationParameters} from 'brayns';
import classnames from 'classnames';
import {isNumber} from 'lodash';

import red from '@material-ui/core/colors/red';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import Typography from '@material-ui/core/Typography';

import {frameToTimeStr} from './utils';


// https://material-ui-next.com/demos/drawers/#mini-variant-drawer
const styles = (theme: Theme) => createStyles({
    container: {
        position: 'relative',
        width: '100%',
        height: 15,
        paddingTop: 5,
        paddingBottom: 5
    },
    containerShift: {
        cursor: 'pointer'
    },
    barContainer: {
        position: 'relative',
        width: '100%',
        height: 5,
        transition: theme.transitions.create('transform', {
            duration: 100,
            easing: 'cubic-bezier(.4, .0, 1, 1)'
        }),
        transform: 'scaleY(0.6)'
    },
    barContainerShift: {
        transform: 'scaleY(1)'
    },
    bar: {
        position: 'absolute',
        width: '100%',
        height: '100%',
        backgroundColor: 'rgba(255, 255, 255, .4)',
        zIndex: 0
    },
    knob: {
        position: 'absolute',
        width: 15,
        height: 15,
        top: 0,
        borderRadius: '50%',
        backgroundColor: red.A400,
        transition: theme.transitions.create('transform', {
            duration: 100,
            easing: 'cubic-bezier(.4, .0, 1, 1)'
        })
    },
    nextProgress: {
        position: 'absolute',
        height: '100%',
        backgroundColor: 'rgba(255, 255, 255, .5)',
        transition: theme.transitions.create('opacity', {
            duration: 100,
            easing: 'cubic-bezier(.4, .0, 1, 1)'
        }),
        opacity: 0,
        zIndex: 512
    },
    nextProgressShift: {
        opacity: 1
    },
    progress: {
        position: 'relative',
        backgroundColor: red.A400,
        transition: theme.transitions.create('width', {
            duration: 100,
            easing: 'ease'
        }),
        height: '100%',
        zIndex: 1024,
        willChange: 'width'
    },
    tooltip: {
        position: 'absolute',
        padding: `${theme.spacing.unit / 2}px ${theme.spacing.unit}px`,
        backgroundColor: theme.palette.background.paper,
        color: theme.palette.text.primary,
        borderRadius: theme.shape.borderRadius,
        zIndex: 2048,
        transition: theme.transitions.create('opacity', {
            duration: 100,
            easing: 'cubic-bezier(.4, .0, 1, 1)'
        }),
        opacity: 0
    },
    tooltipShift: {
        opacity: 1
    },
    noTransition: {
        transition: 'none'
    }
});


// https://material-ui-next.com/guides/typescript/
const style = withStyles(styles, {withTheme: true});


class ProgressBar extends PureComponent<Props, State> {
    state: State = {
        showHandle: false,
        isDragging: false
    };

    progressRef: RefObject<HTMLDivElement> = createRef();
    get progress() {
        const {current} = this.progressRef;
        return current;
    }

    tooltipRef: RefObject<HTMLDivElement> = createRef();
    get tooltip() {
        const {current} = this.tooltipRef;
        return current;
    }

    showHandle = () => {
        this.setState({
            showHandle: true
        });
    }
    hideHandle = () => {
        const {isDragging} = this.state;
        if (!isDragging) {
            this.setState({
                showHandle: false
            });
        }
    }

    bindOnDrag = (evt: ReactMouseEvent) => {
        // Prevent text selection when dragging
        evt.preventDefault();

        document.addEventListener('mousemove', this.drag, false);
        const removeDrag = () => this.removeOnDrag();
        document.addEventListener('mouseup', removeDrag, {
            once: true
        });
    }
    removeOnDrag = () => {
        const {isDragging} = this.state;
        document.removeEventListener('mousemove', this.drag);
        this.setState({
            showHandle: !isDragging,
            isDragging: false
        });
    }

    drag = (evt: MouseEvent) => {
        // Prevent text selection when dragging
        evt.preventDefault();
        const state = this.getNextFrame(evt);
        // Emit the frame change
        this.jumpToFrame(state.nextFrame);
        this.setState({
            ...state,
            isDragging: true
        });
    }

    goToNextFrame = () => {
        this.jumpToFrame(this.state.nextFrame);
    }

    showNextFrame = (evt: ReactMouseEvent<HTMLDivElement>) => {
        const state = this.getNextFrame(evt);
        // Update the next position/frame
        this.setState(state);
    }

    jumpToFrame = (frame?: number) => {
        const {onFrameChange} = this.props;
        if (onFrameChange && isNumber(frame)) {
            onFrameChange(frame);
        }
    }

    getNextFrame = (evt: ReactMouseEvent<HTMLDivElement> | MouseEvent): Pick<State, 'nextFrame' | 'nextPosition'> => {
        if (!this.progress) {
            return {};
        }

        const rect = this.progress.getBoundingClientRect();
        // Don't allow for less than 0 or more than the width of the rect
        const position = Math.max(0, Math.min(evt.clientX - rect.left, rect.width));
        const delta = position / rect.width;
        const frame = Math.floor(delta * this.props.frameCount);

        // Update the next position/frame
        return {
            nextPosition: position,
            nextFrame: frame
        };
    }

    render() {
        const {classes, animationParams, frameCount} = this.props;
        const {
            nextFrame,
            nextPosition,
            showHandle
        } = this.state;

        const currentProgress = getProgress(animationParams, frameCount);
        const currentProgressPosition = this.progressPosition(currentProgress);
        const currentProgressWidth = toPercent(currentProgress);

        return (
            <div
                className={classnames(classes.container, {[classes.containerShift]: showHandle})}
                ref={this.progressRef}
                onMouseEnter={this.showHandle}
                onMouseLeave={this.hideHandle}
                onMouseMove={this.showNextFrame}
                onMouseDown={this.bindOnDrag}
                onClick={this.goToNextFrame}
            >
                <div className={classnames(classes.barContainer, {[classes.barContainerShift]: showHandle})}>
                    <div className={classes.bar} />
                    <div
                        className={classnames(classes.nextProgress, {[classes.nextProgressShift]: showHandle})}
                        style={{width: nextPosition}}
                    />
                    <div
                        className={classnames(classes.progress, {[classes.noTransition]: showHandle})}
                        style={{width: currentProgressWidth}}
                    />
                </div>
                <div
                    className={classnames(classes.knob)}
                    style={knobTransform(currentProgressPosition, showHandle)}
                />
                <div
                    ref={this.tooltipRef}
                    className={classnames(classes.tooltip, {[classes.tooltipShift]: showHandle})}
                    style={this.tooltipPosition(nextPosition)}
                >
                    <Typography variant="caption">
                        {tooltipText(nextFrame, animationParams.dt)}
                    </Typography>
                </div>
            </div>
        );
    }

    private progressPosition(progress: number) {
        if (this.progress) {
            const rect = this.progress.getBoundingClientRect();
            const position = rect.width * progress;
            return position;
        }
        return 0;
    }

    private tooltipPosition(pos?: number) {
        if (this.tooltip) {
            const rect = this.tooltip.getBoundingClientRect();
            return {
                top: -(rect.height + 16),
                marginLeft: rect.width / -2,
                left: pos
            };
        }
        return {};
    }
}


export default style(ProgressBar);


function getProgress(params: Partial<AnimationParameters>, frameCount: number): number {
    if (isNumber(params.current)) {
        return (params.current / frameCount) || 0;
    }
    return 0;
}

function toPercent(num: number): string {
    return `${num * 100}%`;
}

function tooltipText(frame?: number, dt?: number): string {
    return `${frameToTimeStr(frame, dt)}`;
}

function knobTransform(pos: number, show?: boolean): CSSProperties {
    return {
        transform: `scale(${show ? '1' : '0'})`,
        left: `${pos - 7.5}px`
    };
}


interface Props extends WithStyles<typeof styles, true> {
    animationParams: Partial<AnimationParameters>;
    frameCount: number;
    onFrameChange?(frame: number): void;
}

interface State {
    showHandle?: boolean;
    nextPosition?: number;
    nextFrame?: number;
    isDragging?: boolean;
}
