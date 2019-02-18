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
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import TextField from '@material-ui/core/TextField';
import withWidth, {isWidthDown, WithWidth} from '@material-ui/core/withWidth';

import {withLoaders, WithLoaders} from '../../common/client';
import {SlideUp} from '../../common/components';

import ModelLoader, {LoaderDescriptor} from './model-loader';
import {defaultProps, findLoader} from './utils';


const styles = (theme: Theme) => createStyles({
    dialog: {
        [theme.breakpoints.up('sm')]: {
            minWidth: 400
        }
    },
    path: {
        flex: 1
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
        this.setState(state => {
            const name = findLoader(path, loaders!);
            if (name.length && state.loader.name !== name) {
                return {
                    path,
                    loader: {
                        name,
                        properties: defaultProps(name, loaders!)
                    }
                };
            }

            return {path} as any;
        });
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
        const {path, loader} = this.state;

        const fullScreen = isWidthDown('xs', width);
        const hasPath = path.length;
        const canLoadModel = !disabled && hasPath && loader.name.length;

        return (
            <div>
                <Dialog
                    open={open}
                    classes={{paper: classes.dialog}}
                    onClose={this.closeDialog}
                    TransitionComponent={SlideUp}
                    fullScreen={fullScreen}
                >
                    <DialogContent>
                        <FormGroup className={classes.path}>
                            <TextField
                                id="model-path"
                                type="text"
                                value={path}
                                onChange={this.updatePath}
                                label="Path"
                                helperText={hasPath ? '' : 'Please type a model path'}
                                error={!hasPath}
                                fullWidth
                                required
                            />
                        </FormGroup>

                        <ModelLoader
                            value={loader}
                            onChange={this.updateLoader}
                        />
                    </DialogContent>

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
