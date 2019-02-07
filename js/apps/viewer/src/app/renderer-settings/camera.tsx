// tslint:disable: member-ordering
import React, {
    ChangeEvent,
    PureComponent
} from 'react';

import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';

import {withCamera, WithCamera} from '../../common/client';
import {SchemaFields, SelectField} from '../../common/components';


const styles = (theme: Theme) => createStyles({
    root: {
        display: 'flex',
        flexDirection: 'column'
    }
});
const style = withStyles(styles);


class CameraSettings extends PureComponent<Props> {
    changeCameraType = (evt: ChangeEvent<HTMLSelectElement>) => this.props.onCameraTypeChange!(evt.target.value);

    render() {
        const {classes, disabled, camera, params, schema, onCameraParamsChange: onSetCameraParams} = this.props;
        const current = camera ? camera.current : '';
        const types = camera ? camera.types : [];
        const fields = schema ? (
            <SchemaFields
                schema={schema}
                values={params!}
                onChange={onSetCameraParams!}
                disabled={disabled}
            />
        ) : null;

        return (
            <div className={classes.root}>
                <SelectField
                    id="camera-mode"
                    label="Camera mode"
                    options={types}
                    value={current}
                    onChange={this.changeCameraType}
                    disabled={disabled}
                    margin="normal"
                    fullWidth
                />

                {fields}
            </div>
        );
    }
}


export default style(
    withCamera(CameraSettings)
);


interface Props extends WithStyles<typeof styles>, WithCamera {
    disabled?: boolean;
}
