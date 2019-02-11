// tslint:disable: member-ordering
import React, {PureComponent} from 'react';

import {ApplicationParameters} from 'brayns';

import {WithAppParams, withAppParms} from '../../common/client';
import {NumericField} from '../../common/components';


class ImageQuality extends PureComponent<Props> {
    updateJpegCompression = (jpegCompression: number) => this.props.onAppParamsChange!({jpegCompression});
    updateImageStreamFps = (imageStreamFps: number) => this.props.onAppParamsChange!({imageStreamFps});

    render() {
        const {appParams, disabled} = this.props;
        const {jpegCompression, imageStreamFps}: Partial<ApplicationParameters> = appParams || {};

        return (
            <div>
                <NumericField
                    label="Image quality"
                    value={jpegCompression}
                    onChange={this.updateJpegCompression}
                    min={1}
                    max={100}
                    disabled={disabled}
                    margin="normal"
                    fullWidth
                />

                <NumericField
                    label="Image stream fps"
                    value={imageStreamFps}
                    onChange={this.updateImageStreamFps}
                    min={1}
                    max={60}
                    disabled={disabled}
                    margin="normal"
                    fullWidth
                />
            </div>
        );
    }
}

export default withAppParms(ImageQuality);


interface Props extends WithAppParams {
    disabled?: boolean;
}
