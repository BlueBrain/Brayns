import React from 'react';
import ReactDOM from 'react-dom';

// Offline assets through service workers
import {register} from './sw';

// App
import App from './app';
import {ConnectionStatusProvider} from './common/client';

// Theming and styling
import './main.css';

// Entry point for our app
const root = document.getElementById('root') as HTMLElement;
ReactDOM.render((
    <ConnectionStatusProvider>
        <App />
    </ConnectionStatusProvider>
), root);

// Register service worker
register();
