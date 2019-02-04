import {JSONSchema7} from 'json-schema';

export function findSchemaByTitle(schema: JSONSchema7, title: string): JSONSchema7 | undefined {
    const {oneOf} = schema;
    if (Array.isArray(oneOf)) {
        const match = oneOf.find(schema => typeof schema === 'object' && schema.title === title);
        if (match) {
            return match as JSONSchema7;
        }
    }
    return;
}
