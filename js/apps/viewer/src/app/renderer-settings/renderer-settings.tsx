import React, {
    ChangeEvent,
    PureComponent
} from 'react';

import SwipeableViews from 'react-swipeable-views';

import AppBar from '@material-ui/core/AppBar';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import Tab from '@material-ui/core/Tab';
import Tabs from '@material-ui/core/Tabs';

import {withConnectionStatus, WithConnectionStatus} from '../../common/client';
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

    render() {
        const {classes, online} = this.props;
        const {settingsTab} = this.state;
        const disabled = !online;

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
                        <ImageQuality disabled={disabled} />
                    </div>
                </SwipeableViews>
            </div>
        );
    }
}

export default style(
    withConnectionStatus(RendererSettings));


type Props = WithStyles<typeof styles>
    & WithConnectionStatus;

interface State {
    settingsTab?: number;
}
