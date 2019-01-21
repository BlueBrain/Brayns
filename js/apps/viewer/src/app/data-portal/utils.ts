import {
    GET_LOADERS,
    LOADERS_SCHEMA
} from 'brayns';
import {JSONSchema7} from 'json-schema';

import brayns from '../../common/client';

import {LoadersContext} from './provider';
import {LoaderWithSchema} from './types';


export function findLoader(file: File | string, context: LoadersContext) {
    const extension = fileExt(file);

    for (const loader of context.loaders!) {
        const {name, extensions} = loader;
        const canLoad = extensions.includes(extension);
        if (canLoad) {
            return name;
        }
    }

    return '';
}

export function defaultProps(name: string, loaders: LoaderWithSchema[]): object {
    const loader = loaders!.find(item => item.name === name);
    if (loader) {
        const props: {[key: string]: any} = {};
        const {properties} = loader.schema;

        if (properties) {
            for (const [key, schema] of Object.entries(properties)) {
                props[key] = (schema as JSONSchema7).default;
            }
        }

        return props;
    }
    return {};
}

export function needsUserInput(items: Array<File | string>, context: LoadersContext) {
    for (const item of items) {
        let res = true;
        const extension = fileExt(item);

        for (const loader of context.loaders!) {
            const {extensions, schema} = loader;
            const {properties} = schema;
            const canLoad = extensions.includes(extension);
            if (canLoad && (!properties || !Object.keys(properties).length)) {
                res = false;
                break;
            }
        }

        if (res) {
            return true;
        }
    }

    return false;
}


function fileExt(file: File | string) {
    const name = file instanceof File ? file.name : file;
    const extension = name.substr(name.lastIndexOf('.') + 1);
    return extension;
}


export async function getLoadersWithSchema(): Promise<LoaderWithSchema[]> {
    const loaders = await getLoaders();
    const schema = await brayns.request(LOADERS_SCHEMA);
    const schemas = schema.oneOf!;
    return loaders.map(loader => ({
        ...loader,
        schema: schemas.find(s => (s as JSONSchema7).title === loader.name) as JSONSchema7
    }));
}

async function getLoaders() {
    const items = await brayns.request(GET_LOADERS);
    return items.sort((a, b) => {
        const nameA = a.name;
        const nameB = b.name;
        if (nameA > nameB) {
            return 1;
        }
        if (nameA < nameB) {
            return -1;
        }
        return 0;
    });
}
