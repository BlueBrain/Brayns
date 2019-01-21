# Brayns JavaScript/TypeScript Client

> A JavaScript/TypeScript interface for interacting with the [Brayns](https://github.com/BlueBrain/Brayns) rendering service.


# Table of Contents

* [Installation](#installation)
* [Usage](#usage)


### Installation
----------------
You can install this package from [NPM](https://www.npmjs.com):
```bash
npm add rxjs rockets-client brayns
```

Or with [Yarn](https://yarnpkg.com/en):
```bash
yarn add rxjs rockets-client brayns
```

*NOTE*: [TypeScript](https://www.typescriptlang.org) type definitions are bundled with the package.


### Usage
---------
Create a client and connect:
```ts
import {Client} from 'brayns';
import {take} from 'rxjs/operators';

const brayns = new Client('myhost');
await brayns.ready.pipe(take(1))
    .toPromise();
```

Listen to server notifications:
```ts
import {Client, PROGRESS} from 'brayns';

const brayns = new Client('myhost');

brayns.observe(PROGRESS)
    .subscribe(progress => {
        console.log(progress);
    });
```

Send notifications:
```ts
import {CANCEL, Client} from 'brayns';

const brayns = new Client('myhost');
await brayns.ready.pipe(take(1))
    .toPromise();

brayns.notify(CANCEL, {
    id: 1 // some request id
});
```

Make a request:
```ts
import {Client, GET_STATISTICS} from 'brayns';

const brayns = new Client('myhost');
await brayns.ready.pipe(take(1))
    .toPromise();

const statistics = await brayns.request(GET_STATISTICS);
console.log(statistics);
```

Upload a model:
```ts
import {Client} from 'brayns';

const brayns = new Client('myhost');
await brayns.ready.pipe(take(1))
    .toPromise();

// Usually we get the file from user input
const file = new File([]);

await brayns.upload({file});
```
