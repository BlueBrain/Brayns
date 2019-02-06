import React, {
    ChangeEvent,
    MouseEvent,
    PureComponent
} from 'react';

import {FileWithPath} from 'file-selector';
import {memoize} from 'lodash';

import Button from '@material-ui/core/Button';
import Divider from '@material-ui/core/Divider';
import ExpansionPanel from '@material-ui/core/ExpansionPanel';
import ExpansionPanelActions from '@material-ui/core/ExpansionPanelActions';
import ExpansionPanelDetails from '@material-ui/core/ExpansionPanelDetails';
import ExpansionPanelSummary from '@material-ui/core/ExpansionPanelSummary';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import Typography from '@material-ui/core/Typography';
import CheckIcon from '@material-ui/icons/Check';
import ErrorIcon from '@material-ui/icons/ErrorOutline';
import ExpandMoreIcon from '@material-ui/icons/ExpandMore';

import {withLoaders, WithLoaders} from '../../common/client';
import ModelLoader, {LoaderDescriptor} from './model-loader';


const styles = (theme: Theme) => createStyles({
    content: {
        display: 'flex',
        flexDirection: 'column',
        flex: 1
    },
    path: {
        marginRight: theme.spacing.unit * 2,
        maxWidth: 175,
        textOverflow: 'ellipsis',
        overflow: 'hidden',
        flexBasis: '50%'
    },
    column: {
        flexBasis: '25%'
    }
});

const style = withStyles(styles);


export class ModelList extends PureComponent<Props, State> {
    state: State = {
        current: ''
    };

    createPanelChangeHandler = memoize((panel: string) => (evt: ChangeEvent<{}>, expanded: boolean) => {
        this.setState({
            current: expanded ? panel : false
        });
    });

    createSetPanel = memoize((panel: string | undefined) => (evt: MouseEvent) => {
        this.setState({
            current: panel
        });
    });

    createUpdateModelLoader = memoize((file: FileWithPath) => (loader: LoaderDescriptor) => {
        const {models, onChange} = this.props;
        const copy = models.slice(0);
        const index = copy.findIndex(item => item.file.name === file.name);

        if (index !== -1) {
            copy.splice(index, 1, {file, ...loader});
        }

        if (onChange) {
            onChange(copy);
        }
    });

    render() {
        const {classes, models} = this.props;
        const {current} = this.state;

        const count = models.length - 1;
        const panels = models.map((item, index) => {
            const {file} = item;
            const {name, path} = file;
            const expanded = current === name || (current === '' && index === 0);
            const isFirst = index === 0;
            const isLast = index === count;
            const prev = !isFirst ? this.fileNameAtIndex(index - 1) : undefined;
            const next = !isLast ? this.fileNameAtIndex(index + 1) : undefined;

            const divider = count > 0 ? (<Divider />) : null;
            const prevAction = prev ? (
                <Button onClick={this.createSetPanel(prev)}>
                    Previous
                </Button>
            ) : null;
            const nextAction = next ? (
                <Button onClick={this.createSetPanel(next)}>
                    Next
                </Button>
            ) : null;

            const actions = count > 0 ? (
                <ExpansionPanelActions>
                    {prevAction}
                    {nextAction}
                </ExpansionPanelActions>
            ) : null;

            const error = hasError(file, models) ? (
                <ErrorIcon
                    className={classes.column}
                    color="error"
                />
            ) : (
                <CheckIcon
                    className={classes.column}
                    color="primary"
                />
            );

            return (
                <ExpansionPanel
                    key={name}
                    expanded={expanded}
                    onChange={this.createPanelChangeHandler(name)}
                >
                    <ExpansionPanelSummary expandIcon={<ExpandMoreIcon />}>
                        <Typography className={classes.path}>{path}</Typography>
                        {error}
                    </ExpansionPanelSummary>
                    <ExpansionPanelDetails>
                        <ModelLoader
                            className={classes.content}
                            value={item}
                            onChange={this.createUpdateModelLoader(file)}
                        />
                    </ExpansionPanelDetails>
                    {divider}
                    {actions}
                </ExpansionPanel>
            );
        });

        return (
            <div>
                {panels}
            </div>
        );
    }

    private fileNameAtIndex(index: number) {
        const {models} = this.props;
        const item = models[index];
        return item.file.name;
    }
}

export default style(
    withLoaders(ModelList));


export interface ModelWithLoader extends LoaderDescriptor {
    file: FileWithPath;
}


function hasError(file: FileWithPath, models: ModelWithLoader[]) {
    const match = models.find(m => m.file === file);
    return match && !match.name.length;
}


interface Props extends WithStyles<typeof styles>, WithLoaders {
    models: ModelWithLoader[];
    onChange?(models: ModelWithLoader[]): void;
}

interface State {
    current?: string | boolean;
}
