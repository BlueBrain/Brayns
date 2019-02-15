import React, {
    Component,
    createRef,
    RefObject
} from 'react';

import {IMAGE_JPEG} from 'brayns';
import {isNumber, noop} from 'lodash';
import {
    BehaviorSubject,
    Subject,
    Subscription
} from 'rxjs';
import {
    buffer,
    debounceTime,
    mergeMap,
    switchMap,
    throttleTime
} from 'rxjs/operators';

import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';

import brayns, {
    onReady,
    WithAppParams,
    withAppParms
} from '../../common/client';
import {WithRect, withResizeObserver} from '../../common/components';
import {dispatchFps} from '../../common/events';


const styles = (theme: Theme) => createStyles({
    root: {
        position: 'relative',
        display: 'flex',
        flexDirection: 'column',
        height: '100%'
    },
    canvas: {
        flex: 1,
        display: 'flex'
    }
});

const style = withStyles(styles);


export class ImageStream extends Component<Props> {
    // We use this image to decode the jpeg from Blob streams
    private imageRenderFps = new BehaviorSubject(0);

    private dataUri = new Subject<string>();
    private subs: Subscription[] = [];

    private canvasRef: RefObject<HTMLCanvasElement> = createRef();
    get canvas() {
        return this.canvasRef.current;
    }
    get ctx() {
        const canvas = this.canvas;
        if (canvas) {
            const ctx = canvas.getContext('2d');
            return ctx;
        }
        return null;
    }

    updateVieport = (viewport: number[]) => {
        // Use int for coords to avoid extra calculations to create the anti-aliasing effect
        // See https://developer.mozilla.org/en-US/docs/Web/API/Canvas_API/Tutorial/Optimizing_canvas
        const [width, height] = toInt(viewport);

        this.props.onAppParamsChange!({
            viewport: [width, height]
        });

        // Set canvas size
        if (this.ctx) {
            this.ctx.canvas.width = width;
            this.ctx.canvas.height = height;
        }
    }

    componentDidMount() {
        // If there's no container el we can bind the camera to,
        // there's no point in continuing
        if (!this.ctx) {
            return;
        }

        // Create the smooth fps fn
        const getRenderFps = smoothFpsFn();

        this.subs.push(...[
            // Draw new image
            brayns.observe(IMAGE_JPEG)
                .pipe(mergeMap(blobToImg))
                .subscribe(img => {
                    const width = img.naturalWidth;
                    const height = img.naturalHeight;
                    const widthScaleFactor = this.canvas!.width / width;
                    const heightScaleFactor = this.canvas!.height / height;

                    if (this.ctx) {
                        this.ctx.drawImage(img, 0, 0, widthScaleFactor * width, heightScaleFactor * height);
                    }

                    // Calc the fps of image decode (accounts for networking)
                    const fps = getRenderFps();
                    this.imageRenderFps.next(fps);
                    // Schedule uri for revoke
                    this.dataUri.next(img.src);
                }),
            // Update the viewport on window resize
            onReady()
                .pipe(switchMap(() => this.props.rectChanges!))
                .subscribe(rect => {
                    const size = [rect.width, rect.height];
                    // If we try to set viewport props to 0,
                    // Brayns will die
                    if (size.every(num => num > 0)) {
                        this.updateVieport(size);
                    }
                }),
            // Emit image fps updates
            this.imageRenderFps.pipe(throttleTime(750))
                .subscribe(fps => {
                    dispatchFps(fps);
                }),
            this.dataUri.pipe(
                // Cleanup data uris 5s after there is no activity
                buffer(this.imageRenderFps.pipe(debounceTime(5 * 1000))))
                .subscribe(uris => {
                    for (const uri of uris) {
                        URL.revokeObjectURL(uri);
                    }
                })
        ]);
    }

    componentWillUnmount() {
        for (const sub of this.subs) {
            sub.unsubscribe();
        }
    }

    // We use moz-opaque to improve the perf. of the canvas
    // See https://developer.mozilla.org/en-US/docs/Web/API/Canvas_API/Tutorial/Optimizing_canvas
    render() {
        const {classes, rectRef} = this.props;
        return (
            <div ref={rectRef} className={classes.root}>
                <div className={classes.canvas}>
                    <canvas ref={this.canvasRef} moz-opaque="true" />
                </div>
            </div>
        );
    }
}

export default style(
    withAppParms(
        withResizeObserver(ImageStream))
);


function toInt(viewport: number[]) {
    return viewport.map(value => Math.floor(value));
}

// TODO: Add tests
function smoothFpsFn() {
    let lastPaintTime: number;
    const smoothingFactor = 0.9;
    let smoothNom = 0;
    let smoothDen = 0;

    return () => {
        const prevPaintTime = lastPaintTime;
        const now = performance.now();
        let smoothFps = 0;

        if (isNumber(prevPaintTime)) {
            const duration = now - prevPaintTime;
            // Smooth fps
            // See https://github.com/BlueBrain/Brayns/blob/master/brayns/common/Timer.h for source
            smoothNom = smoothNom * smoothingFactor + duration / 1000.0;
            smoothDen = smoothDen * smoothingFactor + 1;
            smoothFps = smoothDen / smoothNom;
        }

        // Update last paint time
        lastPaintTime = now;

        return smoothFps;
    };
}

function blobToImg(blob: Blob) {
    const url = URL.createObjectURL(blob);
    const img: any = new Image();
    return new Promise<HTMLImageElement>(resolve => {
        img.src = url;
        // https://medium.com/dailyjs/image-loading-with-image-decode-b03652e7d2d2
        if (img.decode) {
            img.decode()
                // TODO: Figure out why decode() throws DOMException
                .then(() => resolve(img), noop);
        } else {
            img.onload = () => resolve(img);
        }
    });
}


type Props = WithStyles<typeof styles>
    & WithAppParams
    & WithRect;
