import {JSONSchema7} from 'json-schema';
import {LoaderWithSchema} from '../../common/client';


export function findLoader(file: File | string, loaders: LoaderWithSchema[]) {
    const extension = fileExt(file);

    for (const loader of loaders) {
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

export function needsUserInput(items: Array<File | string>, loaders: LoaderWithSchema[]) {
    for (const item of items) {
        let res = true;
        const extension = fileExt(item);

        for (const loader of loaders) {
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
