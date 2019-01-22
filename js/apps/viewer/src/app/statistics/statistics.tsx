import React, {PureComponent} from 'react';

import {
    GET_STATISTICS,
    SET_STATISTICS,
    Statistics as RendererStatistics
} from 'brayns';

// https://www.npmjs.com/package/bytes
import {format} from 'bytes';
import {animationFrameScheduler, Subscription} from 'rxjs';
import {mergeMap, observeOn} from 'rxjs/operators';

import Paper from '@material-ui/core/Paper';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';

import brayns, {onReady} from '../../common/client';
import {onFpsChange} from '../../common/events';


const styles = (theme: Theme) => createStyles({
    content: {
        ...theme.typography.caption,
        color: theme.palette.text.primary,
        padding: theme.spacing.unit * 2
    },
    list: {
        margin: 0,
        padding: 0,
        listStyle: 'none'
    },
    value: {
        color: theme.palette.primary.main
    }
});

const style = withStyles(styles);


export class Statistics extends PureComponent<Props, State> {
    state: State = {
        imageRenderFps: 0,
        canUpdate: true
    };

    private timers: Map<string, NodeJS.Timer> = new Map();
    private subs: Subscription[] = [];

    update = (key: keyof Pick<State, 'serverFps' | 'imageRenderFps'>, value?: number) => {
        const timer = this.timers.get(key);

        if (timer) {
            clearTimeout(timer);
            this.timers.delete(key);
        }

        if (this.state.canUpdate) {
            this.setState({
                [key]: Math.floor(value || 0)
            });
        }

        this.timers.set(key, setTimeout(() => {
            this.timers.delete(key);
            if (this.state.canUpdate) {
                this.setState({
                    [key]: 0
                });
            }
        }, 1250));
    }

    componentDidMount() {
        this.subs.push(...[
            brayns.observe(SET_STATISTICS)
                .pipe(observeOn(animationFrameScheduler))
                .subscribe(statistics => {
                    this.update('serverFps', statistics.fps);
                    this.setState({statistics});
                }),
            onReady().pipe(mergeMap(() => brayns.request(GET_STATISTICS)))
                .subscribe(statistics => {
                    this.setState({statistics});
                }),
            onFpsChange().pipe(observeOn(animationFrameScheduler))
                .subscribe(fps => {
                    this.update('imageRenderFps', fps);
                })
        ]);
    }

    componentWillUnmount() {
        for (const timer of this.timers.values()) {
            clearTimeout(timer);
        }
        this.timers.clear();

        this.setState({
            canUpdate: false
        });

        while (this.subs.length) {
            const sub = this.subs.pop();
            sub!.unsubscribe();
        }
    }

    render() {
        const {
            statistics,
            imageRenderFps,
            serverFps
        } = this.state;
        const {classes} = this.props;
        const sceneSize = format(statistics ? statistics.sceneSizeInBytes : 0);

        return (
            <Paper className={classes.content}>
                <ul className={classes.list}>
                    <li>Server FPS: <span className={classes.value}>{serverFps || 0}</span></li>
                    <li>Image Rendering FPS: <span className={classes.value}>{imageRenderFps}</span></li>
                    <li>Scene size: <span className={classes.value}>{sceneSize}</span></li>
                </ul>
            </Paper>
        );
    }
}

export default style(Statistics);


type Props = WithStyles<typeof styles>;

interface State {
    statistics?: RendererStatistics;
    imageRenderFps?: number;
    serverFps?: number;
    canUpdate?: boolean;
}
