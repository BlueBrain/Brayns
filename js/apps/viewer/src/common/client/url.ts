import {isObject, isString} from 'lodash';


/**
 * Get renderer settings from url
 */
export function getHostFromUrl(): string {
    // Get config from query params
    const params = parseQueryParams(location.href);
    // Get the renderer service host from the env at build time
    // See https://github.com/facebookincubator/create-react-app/blob/master/packages/react-scripts/template/README.md#adding-custom-environment-variables
    const host = params.host || process.env.REACT_APP_RENDERER_HOST;

    if (isString(host)) {
        return host;
    }

    // Fallback to localhost
    return '127.0.0.1:8200';
}

/**
 * Parse url query params and return them as an object
 */
export function parseQueryParams(str: string): {[key: string]: any} {
    const params: {[key: string]: any} = {};

    try {
        const url = new URL(str);
        const {search, searchParams} = url;

        if (isObject(searchParams)) {
            for (const [key, value] of searchParams as any) {
                params[key] = value;
            }
        } else if (isString(search)) {
            // If URLSearchParams is not available, try to parse the string
            // NOTE: This is a naive impl. of a query url parser
            const parts = search.substr(1)
                .split('&');
            for (const part of parts) {
                const [key, value] = part.split('=');
                params[key] = decodeURIComponent(value);
            }
            return params;
        }
    } catch {} // tslint:disable-line: no-empty

    return params;
}
