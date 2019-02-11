import React, {
    createRef,
    PureComponent,
    RefObject
} from 'react';

import {INSPECT, InspectParams} from 'brayns';
import {fromEvent, Subscription} from 'rxjs';
import {mergeMap, switchMap} from 'rxjs/operators';
import {
    PerspectiveCamera,
    Quaternion,
    Vector3
} from 'three';

import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';

import brayns, {
    CameraCoords,
    ifReady,
    onReady,
    withCamera,
    WithCamera
} from '../../common/client';
import {WithRect, withResizeObserver} from '../../common/components';
import {dispatchNotification} from '../../common/events';
import {isCmdKey} from '../../common/utils';

import {EventType, TrackballControls} from './trackball-controls';


const styles = (theme: Theme) => createStyles({
    root: {
        display: 'flex',
        position: 'relative',
        flex: 1,
        flexDirection: 'column',
        userSelect: 'none'
    },
    canvas: {
        flex: 1,
        display: 'flex'
    }
});

const style = withStyles(styles);


export class Camera extends PureComponent<Props> {
    private camera?: PerspectiveCamera;
    private controls?: TrackballControls;

    private subs: Subscription[] = [];

    private container: RefObject<HTMLDivElement> = createRef();
    get viewport() {
        const {current} = this.container;
        return current;
    }

    inspect = async (evt: MouseEvent) => {
        const viewport = this.viewport;
        if (!viewport) {
            return;
        }

        const {width, height} = viewport.getBoundingClientRect();

        // Only dispatch inspect if the CTRL/CMD key is held down
        if (this.viewport && width > 0 && height > 0 && isCmdKey(evt)) {
            // Do not propagate the event further
            evt.stopPropagation();
            evt.stopImmediatePropagation();

            const box = this.viewport.getBoundingClientRect();
            const coords: InspectParams = [
                (evt.clientX - box.left) / width,
                1 - ((evt.clientY - box.top) / height)
            ];

            try {
                const position = await brayns.request(INSPECT, coords);
                // tslint:disable-next-line: no-console
                console.log(position);
            } catch (err) {
                dispatchNotification(err);
            }
        }
    }

    update = (camera?: CameraCoords) => {
        if (!camera || !this.camera || !this.controls) {
            return;
        }

        const quaternion = new Quaternion(...camera.orientation);
        const position = new Vector3(...camera.position);

        const up = new Vector3(0, 1, 0);
        up.applyQuaternion(quaternion);

        this.camera.position.copy(position);
        this.camera.setRotationFromQuaternion(quaternion);
        this.camera.up.copy(up);

        this.controls.target.fromArray(camera.target ? camera.target : [0, 0, 0]);
    }

    resize(width: number, height: number) {
        if (this.camera && this.controls) {
            // Update PerspectiveCamera frustum aspect ratio
            this.camera.aspect = aspectRatio(width, height);
            this.camera.updateProjectionMatrix();
            // Update controls
            this.controls.handleResize();
        }
    }

    componentDidMount() {
        // If there's no container el we can bind the camera to,
        // there's no point in continuing
        const viewport = this.viewport;
        if (!viewport) {
            return;
        }

        // Attach the inspect event as a native evt listener,
        // otherwise we cannot prevent the trackball controls from also trigerring
        // See https://medium.com/@ericclemmons/react-event-preventdefault-78c28c950e46 for more details
        const container = this.props.rectRef!.current;
        if (container) {
            container.addEventListener('mousedown', this.inspect, true);
        }

        // Create camera and controls
        const camera = new PerspectiveCamera(0, 0, 0, 0);
        const controls = new TrackballControls(camera, viewport);
        // Set initial position for the Camera and target for controls (this is necessary,
        // otherwise the change events on controls will never be triggered).
        const position = new Vector3(0, 0, 1);
        const target = new Vector3(0.5, 0.5, 0.5);
        camera.position.copy(position);
        controls.target.copy(target);

        // Ensure that zooming is not weird
        controls.staticMoving = true;

        // Set camera aspect
        const rect = viewport.getBoundingClientRect();
        this.resize(rect.width, rect.height);

        this.camera = camera;
        this.controls = controls;

        const cameraChange = fromEvent(this.controls, EventType.Change)
            .pipe(mergeMap(() => ifReady({
                position: getPosition(camera),
                orientation: getQuaternion(camera),
                target: getTarget(controls)
            })));

        this.subs.push(...[
            // When camera moves we sync with the renderer camera
            cameraChange.subscribe(change => {
                this.props.onCameraCoordsChange!(change);
            }),
            // Listen to viewport size updates
            onReady()
                .pipe(switchMap(() => this.props.rectChanges!))
                .subscribe(rect => {
                    const {width, height} = rect;
                    if (width > 0 && height > 0) {
                        this.resize(width, height);
                    }
                })
        ]);
    }

    componentDidUpdate() {
        this.update(this.props.camera);
    }

    componentWillUnmount() {
        // Remove the native event listener for inspect
        const container = this.props.rectRef!.current;
        if (container) {
            container.removeEventListener('mousedown', this.inspect, true);
        }

        // Destroy controls
        if (this.controls) {
            this.controls.dispose();
        }
        for (const sub of this.subs) {
            sub.unsubscribe();
        }
    }

    render() {
        const {classes, rectRef} = this.props;
        return (
            <div className={classes.root} ref={rectRef}>
                <div
                    ref={this.container}
                    className={classes.canvas}
                />
            </div>
        );
    }
}

export default style(
    withCamera(
        withResizeObserver(Camera)));


function aspectRatio(width: number, height: number): number {
    return width / height;
}

function getPosition(camera: PerspectiveCamera) {
    const p = camera.position;
    return p.toArray();
}

function getQuaternion(camera: PerspectiveCamera) {
    const q = new Quaternion();
    q.setFromEuler(camera.rotation);
    return q.toArray();
}

function getTarget(controls: TrackballControls) {
    const target = controls.target;
    return target.toArray();
}

type Props = WithStyles<typeof styles>
    & WithCamera
    & WithRect;
