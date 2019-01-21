import React, {ChangeEvent, PureComponent} from 'react';

import {
    GET_RENDERER,
    GET_RENDERER_PARAMS,
    Renderer as RendererType,
    RENDERER_PARAMS,
    RendererParams,
    SCHEMA,
    SET_RENDERER,
    SET_RENDERER_PARAMS
} from 'brayns';
import {JSONSchema7} from 'json-schema';
import {Subscription} from 'rxjs';
import {mergeMap} from 'rxjs/operators';

import brayns, {onReady} from '../../common/client';
import {
    ColorField,
    NumericField,
    SchemaFields,
    SelectField,
    SwitchField
} from '../../common/components';
import {dispatchNotification} from '../../common/events';

import {findSchemaForType} from './utils';


const MAX_ACC_FRAMES = 1000;


export default class Renderer extends PureComponent<Props, State> {
    state: State = {
        current: '',
        backgroundColor: [0, 0, 0],
        maxAccumFrames: 0,
        samplesPerPixel: 1,
        headLight: false
    };

    private subs: Subscription[] = [];

    changeRendererType = async (evt: ChangeEvent<HTMLSelectElement>) => {
        const type = evt.target.value;
        const {paramsSchema} = this.state;
        if (paramsSchema) {
            const schema = findSchemaForType(paramsSchema, type);
            await this.updateRenderer({
                current: type
            });
            const params = await getCurrentRendererParams();
            this.setState({
                current: type,
                currentRendererSchema: schema,
                currentRendererParams: params
            });
        }
    }

    updateSamplesPerPixel = (value: number) => {
        this.updateRenderer({
            samplesPerPixel: computeSPPx(this.state.samplesPerPixel!, value)
        });
    }

    updateMaxAccumFrames = (value: number) => {
        const num = Math.round(value);
        this.updateRenderer({
            maxAccumFrames: num < MAX_ACC_FRAMES
                ? num
                : MAX_ACC_FRAMES
        });
    }

    updateHeadLight = (evt: ChangeEvent<HTMLInputElement>, checked: boolean) => {
        this.updateRenderer({
            headLight: checked
        });
    }

    updateBackgroundColor = (color: number[]) => {
        this.setState({
            backgroundColor: color
        });
        const rgb = toBraynsRgb(color);
        this.updateRenderer({
            backgroundColor: rgb
        }, false);
    }

    updateRenderer = async (props: RendererParams, updateState: boolean = true) => {
        try {
            await brayns.request(SET_RENDERER, props);
            if (updateState) {
                this.setState(toRendererState(props));
            }
        } catch (err) {
            dispatchNotification(err);
        }
    }

    updateRendererParams = async (params: object) => {
        try {
            await brayns.request(SET_RENDERER_PARAMS, params);
            this.setState({
                currentRendererParams: {
                    ...this.state.currentRendererParams,
                    ...params
                }
            });
        } catch (err) {
            dispatchNotification(err);
        }
    }

    componentDidMount() {
        this.subs.push(...[
            brayns.observe(SET_RENDERER)
                .pipe(mergeMap(withRendererParams))
                .subscribe(([renderer, params]) => {
                    const {paramsSchema} = this.state;
                    if (paramsSchema) {
                        this.setState({
                            ...getRendererState(renderer, paramsSchema),
                            currentRendererParams: params
                        });
                    }
                }),
            brayns.observe(SET_RENDERER_PARAMS)
                .subscribe(params => {
                    this.setState({
                        currentRendererParams: params
                    });
                }),
            onReady().pipe(mergeMap(() => getCurrentState()))
                .subscribe(([renderer, schema, params]) => {
                    this.setState({
                        ...getRendererState(renderer, schema),
                        paramsSchema: schema,
                        currentRendererParams: params
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
        const {disabled} = this.props;
        const {
            current,
            currentRendererSchema,
            currentRendererParams,
            backgroundColor,
            samplesPerPixel,
            maxAccumFrames,
            headLight,
            types
        } = this.state;

        const fields = currentRendererSchema ? (
            <SchemaFields
                schema={currentRendererSchema}
                values={currentRendererParams}
                onChange={this.updateRendererParams}
                disabled={disabled}
            />
        ) : null;

        return (
            <div>
                <SelectField
                    id="rendering-mode"
                    label="Rendering mode"
                    options={types}
                    value={current}
                    onChange={this.changeRendererType}
                    disabled={disabled}
                    margin="normal"
                    fullWidth
                />

                <NumericField
                    label="Samples/px"
                    value={samplesPerPixel}
                    onChange={this.updateSamplesPerPixel}
                    type="integer"
                    step={1}
                    disabled={disabled}
                    margin="normal"
                    fullWidth
                />

                <NumericField
                    label="Accumulation frames"
                    value={maxAccumFrames}
                    onChange={this.updateMaxAccumFrames}
                    min={0}
                    max={MAX_ACC_FRAMES}
                    disabled={disabled}
                    margin="normal"
                    fullWidth
                />

                <ColorField
                    id="colorpicker"
                    label="Background"
                    ariaLabel="Open color picker"
                    value={backgroundColor}
                    onChange={this.updateBackgroundColor}
                    margin="normal"
                    disabled={disabled}
                    fullWidth
                />

                <SwitchField
                    id="head-light"
                    label="Head light"
                    value={headLight}
                    onChange={this.updateHeadLight}
                    disabled={disabled}
                    fullWidth
                />

                {fields}
            </div>
        );
    }
}


function getCurrentState(): Promise<CurrentState> {
    return Promise.all([
        brayns.request(GET_RENDERER),
        brayns.request(SCHEMA, {endpoint: RENDERER_PARAMS}),
        getCurrentRendererParams()
    ]) as any;
}

async function withRendererParams(renderer: RendererType): Promise<WithRenderer> {
    const params = await getCurrentRendererParams();
    return [renderer, params];
}

async function getCurrentRendererParams() {
    const params = await brayns.request(GET_RENDERER_PARAMS);
    return params;
}

function getRendererState(renderer: RendererType, paramsSchema: JSONSchema7) {
    const props = withRgbBackgroundColor(renderer);
    const currentRendererSchema = findSchemaForType(paramsSchema, renderer.current);
    return {
        ...props,
        currentRendererSchema
    };
}

function withRgbBackgroundColor(renderer: RendererType): RendererState {
    return {
        ...renderer,
        backgroundColor: toRgb(renderer.backgroundColor)
    };
}

function toRendererState(props: RendererParams): CommonRendererState {
    if (props.backgroundColor) {
        return {
            ...props,
            backgroundColor: toRgb(props.backgroundColor)
        };
    }
    return props as any;
}

function computeSPPx(prev: number, current: number) {
    if (current === 0) {
        return prev > 0 ? -1 : 1;
    }
    return current;
}

// TODO: Add tests?
function toRgb(rgb?: number[]): number[] {
    if (Array.isArray(rgb)) {
        const color = rgb.map(v => Math.floor(v * 255));
        return color;
    }
    return [0, 0, 0];
}

// TODO: Add tests?
function toBraynsRgb(rgb: number[]): number[] {
    return rgb.map(v => v / 255);
}


interface Props {
    disabled?: boolean;
}

type CommonRendererState = RendererParams;

type RendererState = Pick<RendererType, 'types'> & CommonRendererState;

interface State extends Partial<RendererState> {
    paramsSchema?: JSONSchema7;
    currentRendererSchema?: JSONSchema7;
    currentRendererParams?: any;
}

interface CurrentState extends Array<JSONSchema7 | RendererType | object> {
    0: RendererType;
    1: JSONSchema7;
    2: object;
    length: 3;
}

interface WithRenderer extends Array<RendererType | object> {
    0: RendererType;
    1: object;
    length: 2;
}
