// tslint:disable: member-ordering
import React, {
    createRef,
    PureComponent,
    RefObject
} from 'react';

import classNames from 'classnames';
import {Subscription} from 'rxjs';

import IconButton from '@material-ui/core/IconButton';
import {
    createStyles,
    Theme,
    WithStyles,
    withStyles
} from '@material-ui/core/styles';
import ArrowDropDownIcon from '@material-ui/icons/ArrowDropDown';
import ArrowDropUpIcon from '@material-ui/icons/ArrowDropUp';
import ArrowLeftIcon from '@material-ui/icons/ArrowLeft';
import ArrowRightIcon from '@material-ui/icons/ArrowRight';
import Rotate90DegreesCcwIcon from '@material-ui/icons/Rotate90DegreesCcw';

import {withCamera, WithCamera} from '../../common/client';
import {
    Face,
    rotateCamera,
    rotateToTarget,
    Rotation,
    toAxisAngle
} from '../../common/math';

import CubeFace from './face';


const BUTTON_SIZE = 46;
const CUBE_SIZE = 64;
const FACE_TRANSLATION = toPx(CUBE_SIZE / 2);
const DISTORTION_FIX = toPx(CUBE_SIZE);

const styles = (theme: Theme) => createStyles({
    root: {},
    scene: {
        width: CUBE_SIZE,
        height: CUBE_SIZE,
        // https://developer.mozilla.org/en-US/docs/Web/CSS/filter-function/drop-shadow
        filter: 'drop-shadow(0 0 0.5rem rgba(0, 0, 0, .3))',
        perspective: CUBE_SIZE * 2
    },
    cube: {
        width: CUBE_SIZE,
        height: CUBE_SIZE,
        position: 'relative',
        transformStyle: 'preserve-3d',
        transform: `translateZ(-${DISTORTION_FIX})`,
        transition: 'transform .45s',
        willChange: 'transform'
    },
    cubeFace: {
        position: 'absolute'
    },
    frontFace: {
        transform: `rotateY(0deg) translateZ(${FACE_TRANSLATION})`
    },
    topFace: {
        transform: `rotateX(90deg) translateZ(${FACE_TRANSLATION})`
    },
    bottomFace: {
        transform: `rotateX(-90deg) translateZ(${FACE_TRANSLATION})`
    },
    rightFace: {
        transform: `rotateY(90deg) translateZ(${FACE_TRANSLATION})`
    },
    leftFace: {
        transform: `rotateY(-90deg) translateZ(${FACE_TRANSLATION})`
    },
    backFace: {
        transform: `rotateY(180deg) translateZ(${FACE_TRANSLATION})`
    },
    control: {
        position: 'absolute',
        color: '#fff',
        transition: 'opacity .45s ease-in-out',
        transitionDelay: '.15s',
        pointerEvents: 'none',
        opacity: 0
    },
    controlVisible: {
        pointerEvents: 'auto',
        opacity: 1
    },
    controlTop: {
        right: '50%',
        marginRight: -BUTTON_SIZE / 2,
        top: -BUTTON_SIZE
    },
    controlBottom: {
        right: '50%',
        marginRight: -BUTTON_SIZE / 2,
        bottom: -BUTTON_SIZE
    },
    controlRight: {
        top: '50%',
        marginTop: -BUTTON_SIZE / 2,
        right: -BUTTON_SIZE
    },
    controlLeft: {
        top: '50%',
        marginTop: -BUTTON_SIZE / 2,
        left: -BUTTON_SIZE
    },
    controlRotate: {
        right: -BUTTON_SIZE,
        top: -BUTTON_SIZE
    }
});

const style = withStyles(styles);


export class Cube extends PureComponent<Props, State> {
    state: State = {};

    containerRef: RefObject<HTMLDivElement> = createRef();
    get container() {
        const container = this.containerRef.current;
        return container;
    }

    private subs: Subscription[] = [];

    rotateToFace = (target: Face) => async () => {
        this.showControls();
        const {camera} = this.props;
        if (camera) {
            const coords = rotateToTarget(camera, target);
            this.props.onCameraCoordsChange!(coords);
        }
    }

    rotateToFront = this.rotateToFace('front');
    rotateToBack = this.rotateToFace('back');
    rotateToRight = this.rotateToFace('right');
    rotateToLeft = this.rotateToFace('left');
    rotateToTop = this.rotateToFace('top');
    rotateToBottom = this.rotateToFace('bottom');

    rotate = (rotation: Rotation) => () => {
        const {camera} = this.props;
        if (camera) {
            const coords = rotateCamera(camera, rotation);
            this.props.onCameraCoordsChange!(coords);
        }
    }

    rotateXCCW = this.rotate({x: -90});
    rotateXCW = this.rotate({x: 90});
    rotateYCW = this.rotate({y: -90});
    rotateYCCW = this.rotate({y: 90});
    rotateZCW = this.rotate({z: -90});

    showControls = () => {
        if (this.state.showControls) {
            return;
        }
        document.addEventListener('mousedown', this.hideControls, false);
        this.setState({
            showControls: true
        });
    }

    hideControls = (evt: MouseEvent) => {
        const target = evt.target;
        const container = this.container;
        if (!container || !target) {
            return;
        }

        if (!container.contains(target as Node)) {
            document.removeEventListener('mousedown', this.hideControls);
            this.setState({
                showControls: false
            });
        }
    }

    componentWillUnmount() {
        document.removeEventListener('mousedown', this.hideControls);
        while (this.subs.length) {
            const sub = this.subs.pop();
            sub!.unsubscribe();
        }
    }

    render() {
        const {classes, className, camera} = this.props;
        const {showControls} = this.state;
        const rotation = toAxisAngle(camera ? camera : {orientation: [0, 0, 0, 1]} as any);
        const transform = toCssRotation(rotation);

        const controlVisible = {[classes.controlVisible]: showControls};

        return (
            <div ref={this.containerRef} className={classes.root}>
                <div className={classNames(className, classes.scene)}>
                    <div className={classes.cube} style={{transform}}>
                        <CubeFace
                            onClick={this.rotateToFront}
                            label="Front"
                            className={classNames(classes.cubeFace, classes.frontFace)}
                            size={CUBE_SIZE}
                        />
                        <CubeFace
                            onClick={this.rotateToBack}
                            label="Back"
                            className={classNames(classes.cubeFace, classes.backFace)}
                            size={CUBE_SIZE}
                        />
                        <CubeFace
                            onClick={this.rotateToRight}
                            label="Right"
                            className={classNames(classes.cubeFace, classes.rightFace)}
                            size={CUBE_SIZE}
                        />
                        <CubeFace
                            onClick={this.rotateToLeft}
                            label="Left"
                            className={classNames(classes.cubeFace, classes.leftFace)}
                            size={CUBE_SIZE}
                        />
                        <CubeFace
                            onClick={this.rotateToTop}
                            label="Top"
                            className={classNames(classes.cubeFace, classes.topFace)}
                            size={CUBE_SIZE}
                        />
                        <CubeFace
                            onClick={this.rotateToBottom}
                            label="Bottom"
                            className={classNames(classes.cubeFace, classes.bottomFace)}
                            size={CUBE_SIZE}
                        />
                    </div>
                </div>
                <IconButton
                    className={classNames(classes.control, classes.controlTop, controlVisible)}
                    onClick={this.rotateXCCW}
                    aria-label="Rotate on X 90 deg counterclockwise"
                >
                    <ArrowDropDownIcon />
                </IconButton>
                <IconButton
                    className={classNames(classes.control, classes.controlBottom, controlVisible)}
                    onClick={this.rotateXCW}
                    aria-label="Rotate on X 90 deg clockwise"
                >
                    <ArrowDropUpIcon />
                </IconButton>
                <IconButton
                    className={classNames(classes.control, classes.controlRight, controlVisible)}
                    onClick={this.rotateYCCW}
                    aria-label="Rotate on Y 90 deg counterclockwise"
                >
                    <ArrowLeftIcon />
                </IconButton>
                <IconButton
                    className={classNames(classes.control, classes.controlLeft, controlVisible)}
                    onClick={this.rotateYCW}
                    aria-label="Rotate on Y 90 deg clockwise"
                >
                    <ArrowRightIcon />
                </IconButton>
                <IconButton
                    className={classNames(classes.control, classes.controlRotate, controlVisible)}
                    onClick={this.rotateZCW}
                    aria-label="Rotate on Z 90 deg counterclockwise"
                >
                    <Rotate90DegreesCcwIcon />
                </IconButton>
            </div>
        );
    }
}

export default style(
    withCamera(Cube)
);


function toCssRotation(rotation?: number[]) {
    if (Array.isArray(rotation)) {
        const [x, y, z, w] = rotation;
        // NOTE: Y coords are inverted, hence the negate
        // See https://www.w3.org/Talks/2012/0416-CSS-WWW2012/Demos/transforms/demo-rotate3d.html
        return `translateZ(-${DISTORTION_FIX}) rotate3d(${x}, ${-y}, ${z}, ${w}rad)`;
    }
    return `translateZ(-${DISTORTION_FIX}) rotateY(0rad)`;
}

function toPx(num: number): string {
    if (Number.isFinite(num)) {
        return `${num}px`;
    }
    return '0';
}


interface Props extends WithStyles<typeof styles>, WithCamera {
    className?: string;
}

interface State {
    rotation?: number[];
    showControls?: boolean;
}
