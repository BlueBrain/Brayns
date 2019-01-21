import {JSONSchema7} from 'json-schema';

export function findSchemaForType(paramsSchema: JSONSchema7, type: string): JSONSchema7 | undefined {
    const {oneOf} = paramsSchema;
    if (Array.isArray(oneOf)) {
        const schema = oneOf.find(schema => typeof schema === 'object' && schema.title === type);
        if (schema) {
            return schema as JSONSchema7;
        }
    }
    return;
}
