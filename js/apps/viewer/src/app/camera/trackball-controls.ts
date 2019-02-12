import {isFunction} from 'lodash';

import {
    Camera,
    EventDispatcher,
    Quaternion,
    TrackballControls as ThreeTrackballControls,
    Vector2,
    Vector3
} from 'three';

import {supportsPassive} from '../../common/utils';


enum State {
    None = - 1,
    Rotate = 0,
    Zoom = 1,
    Pan = 2,
    TouchRotate = 3,
    TouchZoomPan = 4
}

const eps = 0.000001;

export enum EventType {
    Change = 'change',
    MoveStart = 'movestart',
    MoveEnd = 'moveend',
    ZoomStart = 'zoomstart',
    ZoomEnd = 'zoomend'
}

const usePassive = supportsPassive();


/**
 * Three.js trackball controls
 * See https://threejs.org/examples/misc_controls_trackball.html for an example of behavior.
 * Source: https://github.com/mrdoob/three.js/blob/dev/examples/js/controls/TrackballControls.js
 *
 * @example
 * import {TrackballControls} from '@brayns/trackball-controls';
 * import {PerspectiveCamera} from 'three';
 *
 * const container = document.body;
 * const camera = new PerspectiveCamera();
 * const controls = new TrackballControls(camera, container);
 *
 * controls.addEventListener('change', () => {
 *     console.log('tada');
 * });
 */
export class TrackballControls extends EventDispatcher implements ThreeTrackballControls {
    [key: string]: any;

    object: Camera;
    domElement: HTMLElement;
    enabled = true;
    screen: Screen = {
        height: 0,
        left: 0,
        top: 0,
        width: 0
    };

    noRotate = false;
    rotateSpeed = 1.0;

    noZoom = false;
    zoomSpeed = 1.2;

    panSpeed = 0.3;
    noPan = false;

    noRoll = false;

    staticMoving = false;
    dynamicDampingFactor = 0.2;

    minDistance = 0;
    maxDistance = Infinity;

    keys: number[] = [
        65, // A
        83, // S
        68 // D
    ];

    target = new Vector3();
    target0 = this.target.clone();
    position0: Vector3;
    up0: Vector3;

    private lastPosition: Vector3 = new Vector3();

    private state: State = State.None;
    private prevState: State = State.None;

    private eye = new Vector3();

    private movePrev = new Vector2();
    private moveCurr = new Vector2();

    private lastAxis = new Vector3();
    private lastAngle = 0;

    private zoomStart = new Vector2();
    private zoomEnd = new Vector2();

    private touchZoomDistanceStart = 0;
    private touchZoomDistanceEnd = 0;

    private panStart = new Vector2();
    private panEnd = new Vector2();

    constructor(object: Camera, domElement: HTMLElement) {
        super();

        this.object = object;
        this.domElement = domElement;
        this.position0 = object.position.clone();
        this.up0 = object.up.clone();

        // NOTE: We use instance fns to make sure that `this` when the event handles are invoked referrs to this class
        // Check https://github.com/Microsoft/TypeScript/wiki/'this'-in-TypeScript#use-instance-functions.
        this.domElement.addEventListener('contextmenu', this.contextmenu, false);
        this.domElement.addEventListener('mousedown', this.mousedown, usePassive ? {passive: true} : false);
        this.domElement.addEventListener('wheel', this.mousewheel, usePassive ? {passive: false} : false);

        this.domElement.addEventListener('touchstart', this.touchstart, usePassive ? {passive: true} : false);
        this.domElement.addEventListener('touchend', this.touchend, usePassive ? {passive: true} : false);
        this.domElement.addEventListener('touchmove', this.touchmove, false);

        window.addEventListener('keydown', this.keydown, false);
        window.addEventListener('keyup', this.keyup, false);

        this.handleResize();

        // Force an update at start
        this.update();
    }

    update() {
        this.eye.subVectors(this.object.position, this.target);

        if (!this.noRotate) {
            this.rotateCamera();
        }

        if (!this.noZoom) {
            this.zoomCamera();
        }

        if (!this.noPan) {
            this.panCamera();
        }

        this.object.position.addVectors(this.target, this.eye);
        this.checkDistances();
        this.object.lookAt(this.target);

        if (this.lastPosition.distanceToSquared(this.object.position) > eps) {
            this.dispatchEvent({
                type: EventType.Change
            });
            this.lastPosition.copy(this.object.position);
        }
    }

    reset() {
        this.state = State.None;
        this.prevState = State.None;

        this.target.copy(this.target0);
        this.object.position.copy(this.position0);
        this.object.up.copy(this.up0);

        this.eye.subVectors(this.object.position, this.target);

        this.object.lookAt(this.target);

        this.dispatchEvent({
            type: EventType.Change
        });

        this.lastPosition.copy(this.object.position);
    }

    checkDistances() {
        if (!this.noZoom || !this.noPan) {
            if (this.eye.lengthSq() > this.maxDistance * this.maxDistance) {
                this.object.position.addVectors(this.target, this.eye.setLength(this.maxDistance));
                this.zoomStart.copy(this.zoomEnd);
            }

            if (this.eye.lengthSq() < this.minDistance * this.minDistance) {
                this.object.position.addVectors(this.target, this.eye.setLength(this.minDistance));
                this.zoomStart.copy(this.zoomEnd);
            }
        }
    }

    zoomCamera() {
        let factor;

        if (this.state === State.TouchZoomPan) {
            factor = this.touchZoomDistanceStart / this.touchZoomDistanceEnd;
            this.touchZoomDistanceStart = this.touchZoomDistanceEnd;
            this.eye.multiplyScalar(factor);

        } else {
            factor = 1.0 + (this.zoomEnd.y - this.zoomStart.y) * this.zoomSpeed;

            if (factor !== 1.0 && factor > 0.0) {
                this.eye.multiplyScalar(factor);
            }

            if (this.staticMoving) {
                this.zoomStart.copy(this.zoomEnd);
            } else {
                this.zoomStart.y += (this.zoomEnd.y - this.zoomStart.y) * this.dynamicDampingFactor;
            }
        }
    }

    panCamera() {
        const mouseChange = new Vector2();
        const objectUp = new Vector3();
        const pan = new Vector3();

        mouseChange.copy(this.panEnd).sub(this.panStart);

        if (mouseChange.lengthSq()) {
            mouseChange.multiplyScalar(this.eye.length() * this.panSpeed);

            pan.copy(this.eye).cross(this.object.up).setLength(mouseChange.x);
            pan.add(objectUp.copy(this.object.up).setLength(mouseChange.y));

            this.object.position.add(pan);
            this.target.add(pan);

            if (this.staticMoving) {
                this.panStart.copy(this.panEnd);
            } else {
                this.panStart.add(mouseChange.subVectors(this.panEnd, this.panStart).multiplyScalar(this.dynamicDampingFactor));
            }
        }
    }

    rotateCamera() {
        const axis = new Vector3();
        const quaternion = new Quaternion();
        const eyeDirection = new Vector3();
        const objectUpDirection = new Vector3();
        const objectSidewaysDirection = new Vector3();
        const moveDirection = new Vector3();
        let angle;

        moveDirection.set(this.moveCurr.x - this.movePrev.x, this.moveCurr.y - this.movePrev.y, 0);
        angle = moveDirection.length();

        if (angle) {
            this.eye.copy(this.object.position).sub(this.target);

            eyeDirection.copy(this.eye).normalize();
            objectUpDirection.copy(this.object.up).normalize();
            objectSidewaysDirection.crossVectors(objectUpDirection, eyeDirection).normalize();

            objectUpDirection.setLength(this.moveCurr.y - this.movePrev.y);
            objectSidewaysDirection.setLength(this.moveCurr.x - this.movePrev.x);

            moveDirection.copy(objectUpDirection.add(objectSidewaysDirection));

            axis.crossVectors(moveDirection, this.eye).normalize();

            angle *= this.rotateSpeed;
            quaternion.setFromAxisAngle(axis, angle);

            this.eye.applyQuaternion(quaternion);
            this.object.up.applyQuaternion(quaternion);

            this.lastAxis.copy(axis);
            this.lastAngle = angle;
        } else if (!this.staticMoving && this.lastAngle) {
            this.lastAngle *= Math.sqrt(1.0 - this.dynamicDampingFactor);
            this.eye.copy(this.object.position).sub(this.target);
            quaternion.setFromAxisAngle(this.lastAxis, this.lastAngle);
            this.eye.applyQuaternion(quaternion);
            this.object.up.applyQuaternion(quaternion);
        }

        this.movePrev.copy(this.moveCurr);
    }

    handleResize() {
        // https://developer.mozilla.org/en-US/docs/Web/API/Node/isSameNode
        if (this.domElement.isSameNode(document)) {
            this.screen.left = 0;
            this.screen.top = 0;
            this.screen.width = window.innerWidth;
            this.screen.height = window.innerHeight;
        } else {
            const box = this.domElement.getBoundingClientRect();
            const d = this.domElement!.ownerDocument!.documentElement;
            this.screen.left = box.left + window.pageXOffset - d.clientLeft;
            this.screen.top = box.top + window.pageYOffset - d.clientTop;
            this.screen.width = box.width;
            this.screen.height = box.height;
        }
    }

    handleEvent(evt: any) {
        const {type} = evt;
        if (isFunction(this[type])) {
            this[type](event);
        }
    }

    dispose() {
        this.domElement.removeEventListener('contextmenu', this.contextmenu, false);
        this.domElement.removeEventListener('mousedown', this.mousedown, false);
        this.domElement.removeEventListener('wheel', this.mousewheel, false);

        this.domElement.removeEventListener('touchstart', this.touchstart, false);
        this.domElement.removeEventListener('touchend', this.touchend, false);
        this.domElement.removeEventListener('touchmove', this.touchmove, false);

        document.removeEventListener('mousemove', this.mousemove, false);
        document.removeEventListener('mouseup', this.mouseup, false);

        window.removeEventListener('keydown', this.keydown, false);
        window.removeEventListener('keyup', this.keyup, false);
    }

    private getMouseOnScreen(pageX: number, pageY: number): Vector2 {
        const vector = new Vector2();
        vector.set(
            (pageX - this.screen.left) / this.screen.width,
            (pageY - this.screen.top) / this.screen.height
        );
        return vector;
    }

    private getMouseOnCircle(pageX: number, pageY: number): Vector2 {
        const vector = new Vector2();
        vector.set(
            ((pageX - this.screen.width * 0.5 - this.screen.left) / (this.screen.width * 0.5)),
            ((this.screen.height + 2 * (this.screen.top - pageY)) / this.screen.width)
        );
        return vector;
    }

    private keydown = (event: KeyboardEvent) => {
        if (this.enabled === false) {
            return;
        }

        window.removeEventListener('keydown', this.keydown);

        this.prevState = this.state;

        if (this.state !== State.None) {
            return;
        } else if (event.keyCode === this.keys[State.Rotate] && !this.noRotate) {
            this.state = State.Rotate;
        } else if (event.keyCode === this.keys[State.Zoom] && !this.noZoom) {
            this.state = State.Zoom;
        } else if (event.keyCode === this.keys[State.Pan] && !this.noPan) {
            this.state = State.Pan;
        }
    }

    private keyup = () => {
        if (this.enabled === false) {
            return;
        }
        this.state = this.prevState;
        window.addEventListener('keydown', this.keydown, false);
    }

    private mousedown = (event: MouseEvent) => {
        if (this.enabled === false) {
            return;
        }

        // No need to stop event propagation or prevent when the user clicks
        // event.preventDefault();
        // event.stopPropagation();

        if (this.state === State.None) {
            this.state = event.button;
        }

        if (this.state === State.Rotate && !this.noRotate) {
            this.moveCurr.copy(this.getMouseOnCircle(event.pageX, event.pageY));
            this.movePrev.copy(this.moveCurr);
        } else if (this.state === State.Zoom && !this.noZoom) {
            this.zoomStart.copy(this.getMouseOnScreen(event.pageX, event.pageY));
            this.zoomEnd.copy(this.zoomStart);
        } else if (this.state === State.Pan && !this.noPan) {
            this.panStart.copy(this.getMouseOnScreen(event.pageX, event.pageY));
            this.panEnd.copy(this.panStart);
        }

        document.addEventListener('mousemove', this.mousemove, false);
        document.addEventListener('mouseup', this.mouseup, false);

        this.dispatchEvent({
            type: EventType.MoveStart
        });
    }

    private mousemove = (event: MouseEvent) => {
        if (this.enabled === false) {
            return;
        }

        event.preventDefault();
        event.stopPropagation();

        if (this.state === State.Rotate && !this.noRotate) {
            this.movePrev.copy(this.moveCurr);
            this.moveCurr.copy(this.getMouseOnCircle(event.pageX, event.pageY));
        } else if (this.state === State.Zoom && !this.noZoom) {
            this.zoomEnd.copy(this.getMouseOnScreen(event.pageX, event.pageY));
        } else if (this.state === State.Pan && !this.noPan) {
            this.panEnd.copy(this.getMouseOnScreen(event.pageX, event.pageY));
        }

        this.update();
    }

    private mouseup = () => {
        if (this.enabled === false) {
            return;
        }

        // No need to stop event propagation or prevent when the user clicks
        // event.preventDefault();
        // event.stopPropagation();

        this.state = State.None;

        document.removeEventListener('mousemove', this.mousemove);
        document.removeEventListener('mouseup', this.mouseup);

        this.dispatchEvent({
            type: EventType.MoveEnd
        });
    }

    private mousewheel = (event: MouseWheelEvent) => {
        if (this.enabled === false) {
            return;
        }

        event.preventDefault();
        event.stopPropagation();

        switch (event.deltaMode) {
            case 2:
                // Zoom in pages
                this.zoomStart.y -= event.deltaY * 0.025;
                break;
            case 1:
                // Zoom in lines
                this.zoomStart.y -= event.deltaY * 0.01;
                break;
            default:
                // undefined, 0, assume pixels
                this.zoomStart.y -= event.deltaY * 0.00025;
                break;
        }

        this.dispatchEvent({
            type: EventType.ZoomStart
        });

        this.update();

        this.dispatchEvent({
            type: EventType.ZoomEnd
        });
    }

    private touchstart = (event: TouchEvent) => {
        if (this.enabled === false) {
            return;
        }

        const {touches} = event;

        switch (touches.length) {
            case 1:
                this.state = State.TouchRotate;
                this.moveCurr.copy(this.getMouseOnCircle(touches[0].pageX, touches[0].pageY));
                this.movePrev.copy(this.moveCurr);
                break;
            default: // 2 or more
                this.state = State.TouchZoomPan;
                const dx = touches[0].pageX - touches[1].pageX;
                const dy = touches[0].pageY - touches[1].pageY;
                this.touchZoomDistanceEnd = this.touchZoomDistanceStart = Math.sqrt(dx * dx + dy * dy);

                const x = (touches[0].pageX + touches[1].pageX) / 2;
                const y = (touches[0].pageY + touches[1].pageY) / 2;
                this.panStart.copy(this.getMouseOnScreen(x, y));
                this.panEnd.copy(this.panStart);
                break;
        }

        this.dispatchEvent({
            type: EventType.MoveStart
        });
    }

    private touchmove = (event: TouchEvent) => {
        if (this.enabled === false) {
            return;
        }

        event.preventDefault();
        event.stopPropagation();

        const {touches} = event;

        switch (touches.length) {
            case 1:
                this.movePrev.copy(this.moveCurr);
                this.moveCurr.copy(this.getMouseOnCircle(touches[0].pageX, touches[0].pageY));
                break;
            default: // 2 or more
                const dx = touches[0].pageX - touches[1].pageX;
                const dy = touches[0].pageY - touches[1].pageY;
                this.touchZoomDistanceEnd = Math.sqrt(dx * dx + dy * dy);

                const x = (touches[0].pageX + touches[1].pageX) / 2;
                const y = (touches[0].pageY + touches[1].pageY) / 2;
                this.panEnd.copy(this.getMouseOnScreen(x, y));
                break;
        }

        this.update();
    }

    private touchend = (event: TouchEvent) => {
        if (this.enabled === false) {
            return;
        }

        const {touches} = event;

        switch (touches.length) {
            case 0:
                this.state = State.None;
                break;
            case 1:
                this.state = State.TouchRotate;
                this.moveCurr.copy(this.getMouseOnCircle(touches[0].pageX, touches[0].pageY));
                this.movePrev.copy(this.moveCurr);
                break;
        }

        this.dispatchEvent({
            type: EventType.MoveEnd
        });
    }

    private contextmenu = (event: Event) => {
        if (this.enabled === false) {
            return;
        }
        event.preventDefault();
    }
}


export interface Screen {
    height: number;
    left: number;
    top: number;
    width: number;
}
