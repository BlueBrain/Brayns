import {Loader} from 'brayns';
import {JSONSchema7} from 'json-schema';

export interface LoaderWithSchema extends Loader {
    schema: JSONSchema7;
}
