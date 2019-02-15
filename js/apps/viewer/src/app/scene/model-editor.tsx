import React, {
    ChangeEvent,
    PureComponent
} from 'react';

import {Transformation} from 'brayns';
import {memoize} from 'lodash';

import ExpansionPanel from '@material-ui/core/ExpansionPanel';
import ExpansionPanelDetails from '@material-ui/core/ExpansionPanelDetails';
import ExpansionPanelSummary from '@material-ui/core/ExpansionPanelSummary';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import Typography from '@material-ui/core/Typography';
import ExpandMoreIcon from '@material-ui/icons/ExpandMore';

import {SchemaFields} from '../../common/components';

import ModelProperties, {ModelPropertiesRenderArgs} from './model-properties';
import ModelSettings from './model-settings';
import ModelTransform from './model-transform';
import TransferFunction from './transfer-function';
import {ModelInfo, ModelProps} from './types';


const styles = (theme: Theme) => createStyles({
    panel: {
        borderRadius: '0 !important'
    },
    settings: {
        flex: 1
    },
    noPad: {
        paddingLeft: 0,
        paddingRight: 0
    }
});
const style = withStyles(styles);


export class ModelEditor extends PureComponent<Props, State> {
    state: State = {
        expanded: 'settings',
        props: {}
    };

    createPanelToggleHandler = memoize((panel: string) => (evt: ChangeEvent<{}>, expanded: boolean) => this.setState({
        expanded: expanded ? panel : null
    }));

    createModelPropsRender = memoize((expanded: string | null, disabled?: boolean) => ({
        schema,
        values,
        hasProps,
        update
    }: ModelPropertiesRenderArgs) => {
        const  {classes} = this.props;

        const fields = schema && values ? (
            <SchemaFields
                schema={schema}
                values={values}
                onChange={update}
                disabled={disabled}
            />
        ) : <span />;

        return hasProps ? (
            <ExpansionPanel
                expanded={expanded === 'props'}
                classes={{root: classes.panel}}
                onChange={this.createPanelToggleHandler('props')}
                elevation={0}
            >
                <ExpansionPanelSummary expandIcon={<ExpandMoreIcon />}>
                    <Typography>Properties</Typography>
                </ExpansionPanelSummary>
                <ExpansionPanelDetails>
                    {fields}
                </ExpansionPanelDetails>
            </ExpansionPanel>
        ) : null;
    }, modelPropsRenderKey);

    updateSettings = (settings: Partial<ModelProps>) => {
        this.update(settings);
    }

    updateTranformation = (transformation: Transformation) => {
        this.update({transformation});
    }

    update = (updates: Partial<ModelProps>) => {
        const {onUpdate} = this.props;
        if (onUpdate) {
            onUpdate(updates);
        }
    }

    render() {
        const {
            classes,
            id,
            bounds,
            boundingBox = false,
            visible = false,
            transformation,
            disabled
        } = this.props;
        const {expanded} = this.state;

        const translation = transformation ? transformation.translation : [0, 0, 0];
        const scale = transformation ? transformation.scale : [1, 1, 1];
        const rotation = transformation ? transformation.rotation : [0, 0, 0];
        const rotationCenter = transformation ? transformation.rotationCenter : [0, 0, 0];

        const modelPropsRender = this.createModelPropsRender(expanded, disabled);

        return (
            <div>
                <ExpansionPanel
                    expanded={expanded === 'settings'}
                    classes={{root: classes.panel}}
                    onChange={this.createPanelToggleHandler('settings')}
                    elevation={0}
                >
                    <ExpansionPanelSummary expandIcon={<ExpandMoreIcon />}>
                        <Typography>Settings</Typography>
                    </ExpansionPanelSummary>
                    <ExpansionPanelDetails classes={{root: classes.noPad}}>
                        <ModelSettings
                            className={classes.settings}
                            bounds={bounds}
                            boundingBox={boundingBox}
                            visible={visible}
                            onChange={this.updateSettings}
                            disabled={disabled}
                        />
                    </ExpansionPanelDetails>
                </ExpansionPanel>

                <ExpansionPanel
                    expanded={expanded === 'tf'}
                    classes={{root: classes.panel}}
                    onChange={this.createPanelToggleHandler('tf')}
                    elevation={0}
                >
                    <ExpansionPanelSummary expandIcon={<ExpandMoreIcon />}>
                        <Typography>Transfer function</Typography>
                    </ExpansionPanelSummary>
                    <ExpansionPanelDetails>
                        <TransferFunction
                            id={id!}
                            disabled={disabled}
                        />
                    </ExpansionPanelDetails>
                </ExpansionPanel>

                <ExpansionPanel
                    expanded={expanded === 'transform'}
                    classes={{root: classes.panel}}
                    onChange={this.createPanelToggleHandler('transform')}
                    elevation={0}
                >
                    <ExpansionPanelSummary expandIcon={<ExpandMoreIcon />}>
                        <Typography>Transform</Typography>
                    </ExpansionPanelSummary>
                    <ExpansionPanelDetails>
                        <ModelTransform
                            rotation={rotation}
                            rotationCenter={rotationCenter}
                            scale={scale}
                            translation={translation}
                            onChange={this.updateTranformation}
                            disabled={disabled}
                        />
                    </ExpansionPanelDetails>
                </ExpansionPanel>

                <ModelProperties id={id!}>
                    {modelPropsRender}
                </ModelProperties>
            </div>
        );
    }
}


export default style(ModelEditor);


function modelPropsRenderKey(expanded?: string | null, disabled?: boolean) {
    return `${expanded}:${disabled}`;
}


interface Props extends WithStyles<typeof styles>,
    Partial<ModelProps>,
    Partial<ModelInfo> {
    disabled?: boolean;
    onUpdate?(updates: Partial<ModelProps>): void;
}

interface State {
    expanded: string | null;
    props?: object;
}
