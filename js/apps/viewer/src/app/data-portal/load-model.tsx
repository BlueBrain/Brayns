import React, {
    ChangeEvent,
    PureComponent
} from 'react';

import {ModelParams} from 'brayns';

import Button from '@material-ui/core/Button';
import Dialog from '@material-ui/core/Dialog';
import DialogActions from '@material-ui/core/DialogActions';
import DialogContent from '@material-ui/core/DialogContent';
import FormGroup from '@material-ui/core/FormGroup';
import List from '@material-ui/core/List';
import ListItem from '@material-ui/core/ListItem';
import ListItemIcon from '@material-ui/core/ListItemIcon';
import ListItemSecondaryAction from '@material-ui/core/ListItemSecondaryAction';
import ListItemText from '@material-ui/core/ListItemText';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import Switch from '@material-ui/core/Switch';
import TextField from '@material-ui/core/TextField';
import withWidth, {isWidthDown, WithWidth} from '@material-ui/core/withWidth';
import VisibilityIcon from '@material-ui/icons/Visibility';

import {withLoaders, WithLoaders} from '../../common/client';
import {SlideUp, VectorSquareIcon} from '../../common/components';

import ModelLoader, {LoaderDescriptor} from './model-loader';
import {defaultProps, findLoader} from './utils';


const styles = (theme: Theme) => createStyles({
    dialog: {
        [theme.breakpoints.up('sm')]: {
            minWidth: 400
        }
    },
    form: {},
    path: {
        marginBottom: theme.spacing.unit,
        flex: 1
    },
    loader: {
        marginTop: theme.spacing.unit * 2
    },
    list: {
        paddingTop: 0
    }
});

const style = withStyles(styles);


export class LoadModel extends PureComponent<Props, State> {
    state: State = {
        path: '',
        boundingBox: false,
        visible: true,
        loader: {
            name: '',
            properties: {}
        }
    };

    loadModel = async () => {
        const {onPathLoad} = this.props;
        const {path, boundingBox, visible, loader} = this.state;
        const params = {
            path,
            boundingBox,
            visible,
            loaderName: loader.name,
            loaderProperties: loader.properties
        };

        if (onPathLoad) {
            onPathLoad(params);
        }

        this.closeDialog();
    }

    closeDialog = () => {
        const {onClose} = this.props;
        if (onClose) {
            onClose();
        }
    }

    updatePath = (evt: ChangeEvent<HTMLInputElement>) => {
        const path = evt.target.value;
        const {loaders} = this.props;
        this.setState({
            path,
            loader: {
                name: findLoader(path, loaders!),
                properties: defaultProps(name, loaders!)
            }
        });
    }

    updateProps = (prop: keyof ModelParams) => (evt: ChangeEvent<HTMLInputElement>, checked: boolean) => {
        this.setState({
            [prop]: checked
        } as any);
    }

    updateLoader = (loader: LoaderDescriptor) => {
        this.setState({loader});
    }

    render() {
        const {
            disabled,
            open,
            classes,
            width
        } = this.props;
        const {
            path,
            visible,
            boundingBox,
            loader
        } = this.state;

        const fullScreen = isWidthDown('xs', width);
        const canLoadModel = !disabled && path.length && loader.name.length;

        return (
            <div>
                <Dialog
                    open={open}
                    classes={{paper: classes.dialog}}
                    onClose={this.closeDialog}
                    TransitionComponent={SlideUp}
                    fullScreen={fullScreen}
                >
                    <DialogContent className={classes.form}>
                        <FormGroup className={classes.path}>
                            <TextField
                                id="model-path"
                                type="text"
                                value={path}
                                onChange={this.updatePath}
                                label="Path"
                                error={!path.length}
                                fullWidth
                                required
                            />
                        </FormGroup>

                        <ModelLoader
                            className={classes.loader}
                            value={loader}
                            onChange={this.updateLoader}
                        />
                    </DialogContent>
                    <List className={classes.list}>
                        <ListItem>
                            <ListItemIcon>
                                <VectorSquareIcon />
                            </ListItemIcon>
                            <ListItemText primary="Bounding box" />
                            <ListItemSecondaryAction>
                                <Switch
                                    onChange={this.updateProps('boundingBox')}
                                    checked={boundingBox}
                                />
                            </ListItemSecondaryAction>
                        </ListItem>
                        <ListItem>
                            <ListItemIcon>
                                <VisibilityIcon />
                            </ListItemIcon>
                            <ListItemText primary="Visible" />
                            <ListItemSecondaryAction>
                                <Switch
                                    onChange={this.updateProps('visible')}
                                    checked={visible}
                                />
                            </ListItemSecondaryAction>
                        </ListItem>
                    </List>
                    <DialogActions>
                        <Button onClick={this.closeDialog}>
                            Cancel
                        </Button>
                        <Button
                            onClick={this.loadModel}
                            color="primary"
                            disabled={!canLoadModel}
                        >
                            Load
                        </Button>
                    </DialogActions>
                </Dialog>
            </div>
        );
    }
}

export default withWidth()(
    style(
        withLoaders(LoadModel)));


interface Props extends WithStyles<typeof styles>, WithWidth, WithLoaders {
    disabled?: boolean;
    open: boolean;
    onPathLoad?(params: Partial<ModelParams>): void;
    onClose?(): void;
}

interface State extends Partial<ModelParams> {
    path: string;
    loader: LoaderDescriptor;
}
