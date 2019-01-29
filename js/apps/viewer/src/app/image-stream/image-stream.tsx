import React, {
    createRef,
    PureComponent,
    RefObject
} from 'react';

import {IMAGE_JPEG, SET_APP_PARAMS} from 'brayns';
import {isNumber} from 'lodash';
import {
    animationFrameScheduler,
    BehaviorSubject,
    Subject,
    Subscription
} from 'rxjs';
import {
    buffer,
    debounceTime,
    mergeMap,
    observeOn,
    throttleTime
} from 'rxjs/operators';

import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';

import brayns, {ifReady, onReady} from '../../common/client';
import {dispatchFps, onAppParamsChange} from '../../common/events';


const styles = (theme: Theme) => createStyles({
    root: {
        display: 'flex',
        position: 'relative',
        flex: 1,
        flexDirection: 'column'
    },
    canvas: {
        flex: 1,
        display: 'flex'
    }
});

const style = withStyles(styles);


export class ImageStream extends PureComponent<Props> {
    private containerRef: RefObject<HTMLDivElement> = createRef();
    private canvasRef: RefObject<HTMLCanvasElement> = createRef();

    // We use this image to decode the jpeg from Blob streams
    private imageRenderFps = new BehaviorSubject(0);

    private dataUri = new Subject<string>();
    private subs: Subscription[] = [];

    get viewport() {
        const {current} = this.containerRef;
        return current;
    }

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

    componentDidMount() {
        // If there's no container el we can bind the camera to,
        // there's no point in continuing
        if (!this.viewport || !this.ctx) {
            return;
        }

        // Use an offscreen canvas for drawing the image before we dump it in the DOM
        const canvas = document.createElement('canvas');
        const ctx = canvas.getContext('2d') as CanvasRenderingContext2D;

        // Create the smooth fps fn
        const getRenderFps = smoothFpsFn();

        const updateVieport = (viewport: number[]) => {
            // Use int for coords to avoid extra calculations to create the anti-aliasing effect
            // See https://developer.mozilla.org/en-US/docs/Web/API/Canvas_API/Tutorial/Optimizing_canvas
            const [width, height] = toInt(viewport);

            brayns.notify(SET_APP_PARAMS, {
                viewport: [width, height]
            });

            // Set canvas/image size
            if (this.ctx) {
                this.ctx.canvas.width = width;
                this.ctx.canvas.height = height;
                canvas.width = width;
                canvas.height = height;
            }
        };

        this.subs.push(...[
            // Resize viewport
            onReady().subscribe(() => {
                const viewport = this.viewport;
                if (viewport) {
                    const rect = viewport.getBoundingClientRect();
                    updateVieport([rect.width, rect.height]);
                }
            }),
            // Draw new image
            brayns.observe(IMAGE_JPEG)
                .pipe(observeOn(animationFrameScheduler), mergeMap(blobToImg))
                .subscribe(img => {
                    const width = img.naturalWidth;
                    const height = img.naturalHeight;
                    const widthScaleFactor = this.canvas!.width / width;
                    const heightScaleFactor = this.canvas!.height / height;
                    canvas.width = width;
                    canvas.height = height;

                    // Draw image on the offscreen canvas
                    ctx.drawImage(img, 0, 0);

                    // Schedule uri for revoke
                    this.dataUri.next(img.src);

                    if (this.ctx) {
                        this.ctx.drawImage(canvas, 0, 0, widthScaleFactor * width, heightScaleFactor * height);
                    }

                    // Calc the fps of decode + paint of image and emit
                    const fps = getRenderFps();
                    this.imageRenderFps.next(fps);
                }),
            // Update the canvas (the offscreen one as well),
            // image size (on the server as well) and renderer viewport.
            onAppParamsChange()
                .pipe(mergeMap(params => ifReady(params.viewport)))
                .subscribe(viewport => {
                    if (viewport) {
                        updateVieport(viewport);
                    }
                }),
            // Emit image fps updates
            this.imageRenderFps.pipe(throttleTime(750))
                .subscribe(fps => {
                    dispatchFps(fps);
                }),
            this.dataUri.pipe(
                // Cleanup data uris 5s after there is no activity
                buffer(this.imageRenderFps.pipe(debounceTime(5 * 1000))),
                observeOn(animationFrameScheduler))
                .subscribe(uris => {
                    for (const uri of uris) {
                        // Release memory for image after canvas paint
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

    // We use moz-opaque to improve perf. of the canvas
    // See https://developer.mozilla.org/en-US/docs/Web/API/Canvas_API/Tutorial/Optimizing_canvas
    render() {
        const {classes} = this.props;
        return (
            <div className={classes.root}>
                <div ref={this.containerRef} className={classes.canvas}>
                    <canvas ref={this.canvasRef} moz-opaque="true" />
                </div>
            </div>
        );
    }
}

export default style(ImageStream);


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
                .then(() => resolve(img));
        } else {
            img.onload = () => resolve(img);
        }
    });
}


type Props = WithStyles<typeof styles>;
