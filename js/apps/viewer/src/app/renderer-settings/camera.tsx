// tslint:disable: member-ordering
import React, {
    ChangeEvent,
    PureComponent
} from 'react';

import {
    Camera,
    CAMERA_PARAMS,
    CameraParams,
    GET_CAMERA,
    GET_CAMERA_PARAMS,
    SCHEMA,
    SET_CAMERA,
    SET_CAMERA_PARAMS
} from 'brayns';
import {JSONSchema7} from 'json-schema';
import {Subscription} from 'rxjs';
import {mergeMap} from 'rxjs/operators';

import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';

import brayns, {onReady} from '../../common/client';
import {SchemaFields, SelectField} from '../../common/components';
import {dispatchNotification} from '../../common/events';

import {findSchemaForType} from './utils';


const styles = (theme: Theme) => createStyles({
    root: {
        display: 'flex',
        flexDirection: 'column'
    }
});
const style = withStyles(styles);


class CameraSettings extends PureComponent<Props, State> {
    state: State = {
        current: ''
    };

    private subs: Subscription[] = [];

    changeCameraType = async (evt: ChangeEvent<HTMLSelectElement>) => {
        const type = evt.target.value;
        const {paramsSchema} = this.state;
        if (paramsSchema) {
            const schema = findSchemaForType(paramsSchema, type);
            await this.updateCameraType(type);
            const params = await getCurrentCameraParams();
            this.setState({
                current: type,
                currentCameraSchema: schema,
                currentCameraParams: params
            });
        }
    }

    updateCameraType = async (type: string) => {
        try {
            const params = {current: type};
            await brayns.request(SET_CAMERA, params);
            this.setState(params);
        } catch (err) {
            dispatchNotification(err);
        }
    }

    updateCameraParams = async (params: object) => {
        try {
            await brayns.request(SET_CAMERA_PARAMS, params);
            this.setState(state => ({
                currentCameraParams: {
                    ...state.currentCameraParams,
                    ...params
                }
            }));
        } catch (err) {
            dispatchNotification(err);
        }
    }

    componentDidMount() {
        this.subs.push(...[
            brayns.observe(SET_CAMERA)
                .pipe(mergeMap(withCameraParams))
                .subscribe(([camera, params]) => {
                    const {paramsSchema} = this.state;
                    if (paramsSchema) {
                        this.setState({
                            ...getCameraState(camera, paramsSchema),
                            currentCameraParams: params
                        });
                    }
                }),
            brayns.observe(SET_CAMERA_PARAMS)
                .subscribe(params => {
                    this.setState({
                        currentCameraParams: params
                    });
                }),
            onReady().pipe(mergeMap(() => getCurrentState()))
                .subscribe(([camera, schema, params]) => {
                    this.setState({
                        ...getCameraState(camera, schema),
                        paramsSchema: schema,
                        currentCameraParams: params
                    });
                })
        ]);
    }

    componentWillUnmount() {
        while (this.subs.length) {
            const sub = this.subs.pop();
            sub!.unsubscribe();
        }
    }

    render() {
        const {classes, disabled} = this.props;
        const {
            current,
            currentCameraSchema,
            currentCameraParams,
            types
        } = this.state;

        const fields = currentCameraSchema ? (
            <SchemaFields
                schema={currentCameraSchema}
                values={currentCameraParams}
                onChange={this.updateCameraParams}
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


export default style(CameraSettings);


function getCurrentState(): Promise<CurrentState> {
    return Promise.all([
        brayns.request(GET_CAMERA),
        brayns.request(SCHEMA, {endpoint: CAMERA_PARAMS}),
        getCurrentCameraParams()
    ]) as any;
}

async function withCameraParams(camera: Camera): Promise<WithCamera> {
    const params = await getCurrentCameraParams();
    return [camera, params];
}

async function getCurrentCameraParams() {
    const params = await brayns.request(GET_CAMERA_PARAMS);
    return params;
}

function getCameraState(camera: Camera, paramsSchema: JSONSchema7) {
    const currentCameraSchema = findSchemaForType(paramsSchema, camera.current);
    return {
        ...camera,
        currentCameraSchema
    };
}


interface Props extends WithStyles<typeof styles> {
    disabled?: boolean;
}

type CameraState = Pick<Camera, 'types'> & CameraParams;

interface State extends Partial<CameraState> {
    paramsSchema?: JSONSchema7;
    currentCameraSchema?: JSONSchema7;
    currentCameraParams?: any;
}

interface CurrentState extends Array<JSONSchema7 | Camera | object> {
    0: Camera;
    1: JSONSchema7;
    2: object;
    length: 3;
}

interface WithCamera extends Array<Camera | object> {
    0: Camera;
    1: object;
    length: 2;
}
