// tslint:disable: no-empty max-classes-per-file member-ordering
import React, {
    Component,
    ComponentType,
    createContext,
    PureComponent
} from 'react';

import {
    ApplicationParameters,
    GET_APP_PARAMS,
    SET_APP_PARAMS
} from 'brayns';
import {debounce} from 'lodash';
import {Subscription} from 'rxjs';
import {mergeMap} from 'rxjs/operators';

import brayns, {onReady} from './client';


export const AppParamsContext = createContext<AppParamsContextValue>({
    appParams: {},
    onAppParamsChange: () => {}
});

export class AppParamsProvider extends Component<{}, State> {
    state: State = {
        appParams: {}
    };

    private subs: Subscription[] = [];

    changeAppParams = (params: Partial<ApplicationParameters>) => {
        this.syncAppParams(params);
        this.setState(state => ({
            appParams: {
                ...state.appParams as ApplicationParameters,
                ...params
            }
        }));
    }
    syncAppParams = debounce((params: Partial<ApplicationParameters>) => {
        brayns.notify(SET_APP_PARAMS, params);
    }, 250);

    componentDidMount() {
        this.subs.push(...[
            onReady().pipe(mergeMap(() => brayns.request(GET_APP_PARAMS)))
                .subscribe(appParams => {
                    this.setState({appParams});
                }),
            brayns.observe(SET_APP_PARAMS)
                .subscribe(appParams => {
                    this.setState({appParams});
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
        const {children} = this.props;
        const {appParams} = this.state;
        const context: AppParamsContextValue = {
            appParams,
            onAppParamsChange: this.changeAppParams
        };
        return (
            <AppParamsContext.Provider value={context}>
                {children}
            </AppParamsContext.Provider>
        );
    }
}


export function withAppParms<P>(Component: ComponentType<P & WithAppParams>): ComponentType<P & WithAppParams> {
    return class extends PureComponent<P & WithAppParams> {
        render() {
            const childFn = (context: WithAppParams) => (
                <Component
                    {...this.props}
                    {...context}
                />
            );
            return (
                <AppParamsContext.Consumer>
                    {childFn}
                </AppParamsContext.Consumer>
            );
        }
    };
}

interface State {
    appParams?: Partial<ApplicationParameters>;
}

export type WithAppParams = Partial<AppParamsContextValue>;

export interface AppParamsContextValue extends State {
    onAppParamsChange: ChangeAppParamsFn;
}

export type ChangeAppParamsFn = (props: Partial<ApplicationParameters>) => void;
