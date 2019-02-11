// tslint:disable: max-classes-per-file member-ordering
import React, {
    Component,
    ComponentType,
    createContext,
    PureComponent
} from 'react';
import {Subscription} from 'rxjs';
import brayns from './client';


export const ConnectionStatusContext = createContext<State>({
    online: false
});


export class ConnectionStatusProvider extends Component<{}, State> {
    state: State = {
        online: false
    };

    private subs: Subscription[] = [];

    componentDidMount() {
        this.subs.push(...[
            brayns.ready.subscribe(online => this.setState({online}))
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
        const context = {
            online: this.state.online
        };
        return (
            <ConnectionStatusContext.Provider value={context}>
                {children}
            </ConnectionStatusContext.Provider>
        );
    }
}


export function withConnectionStatus<P>(Component: ComponentType<P & WithConnectionStatus>): ComponentType<P & WithConnectionStatus> {
    return class extends PureComponent<P & WithConnectionStatus> {
        render() {
            const childFn = (context: WithConnectionStatus) => (
                <Component
                    {...this.props}
                    {...context}
                />
            );
            return (
                <ConnectionStatusContext.Consumer>
                    {childFn}
                </ConnectionStatusContext.Consumer>
            );
        }
    };
}


export type WithConnectionStatus = Partial<State>;

interface State {
    online?: boolean;
}
