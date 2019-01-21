import React, {ChangeEvent, PureComponent} from 'react';

import {
    ApplicationParameters,
    GET_APP_PARAMS,
    SET_APP_PARAMS
} from 'brayns';

import SwipeableViews from 'react-swipeable-views';
import {Subscription} from 'rxjs';
import {mergeMap} from 'rxjs/operators';

import AppBar from '@material-ui/core/AppBar';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import Tab from '@material-ui/core/Tab';
import Tabs from '@material-ui/core/Tabs';

import brayns, {onReady} from '../../common/client';
import storage from '../../common/storage';

import CameraSettings from './camera';
import ImageQuality from './image-quality';
import Renderer from './renderer';


const SELECTED_TAB_KEY = 'tab';


const styles = (theme: Theme) => createStyles({
    tabContent: {
        padding: `0 ${theme.spacing.unit * 3}px`
    },
    tabRoot: {
        minWidth: 82
    }
});

const style = withStyles(styles);


class RendererSettings extends PureComponent<Props, State> {
    state: State = {
        settingsTab: storage.get<number>(SELECTED_TAB_KEY) || 0
    };

    private subs: Subscription[] = [];

    onTabChange = (evt: ChangeEvent<{}>, tab: number) => {
        storage.set(SELECTED_TAB_KEY, tab);
        this.setState({
            settingsTab: tab
        });
    }
    onSwipe = (index: number) => {
        storage.set(SELECTED_TAB_KEY, index);
        this.setState({
            settingsTab: index
        });
    }

    updateAppParams = (params: Partial<ApplicationParameters>): void => {
        brayns.notify(SET_APP_PARAMS, params);
        this.setState({
            appParams: {
                ...this.state.appParams,
                ...params
            }
        });
    }

    // TODO: Move each observer into its own component!
    componentDidMount() {
        // Listen to some events
        this.subs.push(...[
            brayns.observe(SET_APP_PARAMS)
                .subscribe(params => {
                    this.setState({
                        appParams: params
                    });
                }),
            onReady().pipe(mergeMap(() => brayns.request(GET_APP_PARAMS)))
                .subscribe(appParams => {
                    this.setState({
                        appParams
                    });
                })
        ]);
    }
    componentWillUnmount() {
        for (const sub of this.subs) {
            sub.unsubscribe();
        }
    }

    render() {
        const {classes, disabled} = this.props;
        const {
            appParams = {},
            settingsTab
        } = this.state;

        return (
            <div>
                <AppBar position="static" elevation={0} color="default">
                    <Tabs
                        value={settingsTab}
                        onChange={this.onTabChange}
                        indicatorColor="primary"
                        textColor="primary"
                        scrollButtons="on"
                        variant="scrollable"
                    >
                        <Tab classes={{root: classes.tabRoot}} label="Renderer" />
                        <Tab classes={{root: classes.tabRoot}} label="Camera" />
                        <Tab classes={{root: classes.tabRoot}} label="Image" />
                    </Tabs>
                </AppBar>
                <SwipeableViews axis={'x'} index={settingsTab} onChangeIndex={this.onSwipe}>
                    <div className={classes.tabContent}>
                        <Renderer disabled={disabled} />
                    </div>
                    <div className={classes.tabContent}>
                        <CameraSettings disabled={disabled} />
                    </div>
                    <div className={classes.tabContent}>
                        <ImageQuality
                            jpegCompression={appParams.jpegCompression}
                            imageStreamFps={appParams.imageStreamFps}
                            onChange={this.updateAppParams}
                            disabled={disabled}
                        />
                    </div>
                </SwipeableViews>
            </div>
        );
    }
}

export default style(RendererSettings);


interface Props extends WithStyles<typeof styles> {
    disabled?: boolean;
}

interface State {
    appParams?: Partial<ApplicationParameters>;
    settingsTab?: number;
}
