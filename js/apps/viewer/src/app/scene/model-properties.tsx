import React, {
    Fragment,
    PureComponent
} from 'react';

import {
    GET_MODEL_PROPERTIES,
    MODEL_PROPERTIES_SCHEMA,
    SET_MODEL_PROPERTIES
} from 'brayns';
import {JSONSchema7} from 'json-schema';
import {isFunction} from 'lodash';
import {Subscription} from 'rxjs';
import {filter, mergeMap} from 'rxjs/operators';

import brayns, {onReady} from '../../common/client';
import {dispatchNotification} from '../../common/events';

export default class ModelProperties extends PureComponent<Props, State> {
    state: State = {};

    private subs: Subscription[] = [];

    componentDidMount() {
        this.subs.push(...[
            brayns.observe(SET_MODEL_PROPERTIES)
                .pipe(filter(props => props.id === this.props.id))
                .subscribe(({properties}) => {
                    this.setState({
                        values: properties
                    });
                }),
            onReady().pipe(mergeMap(() => getCurrentState(this.props.id)))
                .subscribe(state => {
                    this.setState(state);
                })
        ]);
    }

    componentWillUnmount() {
        while (this.subs.length) {
            const sub = this.subs.pop();
            sub!.unsubscribe();
        }
    }

    renderChildren = () => {
        const {children} = this.props;
        const {schema, values} = this.state;
        const hasProps = schema && schema.properties && Object.keys(schema.properties).length > 0;
        if (isFunction(children)) {
            return children({
                schema,
                values,
                hasProps,
                update: this.update
            });
        }
        return null;
    }

    update = async (properties: object) => {
        const {id} = this.props;
        this.setState(state => ({
            values: {
                ...state.values,
                ...properties
            }
        }), async () => {
            try {
                await brayns.request(SET_MODEL_PROPERTIES, {
                    id,
                    properties
                });
            } catch (err) {
                dispatchNotification(err);
            }
        });
    }

    render() {
        const children = this.renderChildren();
        return (
            <Fragment>
                {children}
            </Fragment>
        );
    }
}


async function getCurrentState(id: string | number): Promise<State> {
    const schema = await brayns.request(MODEL_PROPERTIES_SCHEMA, {id});
    const values = await brayns.request(GET_MODEL_PROPERTIES, {id});
    return {schema, values};
}


interface Props {
    id: string | number;
    children?(args: ModelPropertiesRenderArgs): JSX.Element | null;
}

export interface ModelPropertiesRenderArgs extends State {
    hasProps: boolean;
    update(props: object): void;
}

interface State {
    schema?: JSONSchema7;
    values?: object;
}
