/* Copyright (c) 2014-2015, Human Brain Project
 *                          Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of RenderingResourceManager
 * <https://github.com/BlueBrain/RenderingResourceManager>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

'use strict';

var braynsUrl = 'http://128.178.97.241:5000/';

// constants
var SESSION_STATUS_STOPPED = 0;
var SESSION_STATUS_SCHEDULED = 1;
var SESSION_STATUS_STARTING = 2;
var SESSION_STATUS_RUNNING = 3;
var SESSION_STATUS_STOPPING = 4;

// Variables
var firstCameraRetrieved = false;
var rendering = false;
var camera, controls;
var cameraMatrix = new THREE.Matrix4();
var positionChangeCounter = 0;
var renderedImage = document.getElementById('renderedImage');
var currentCameraPos = new THREE.Vector3(0,0,0);
var initialCameraPos = new THREE.Vector3(0,0,0);
var initialCameraTarget = new THREE.Vector3(0,0,0);
var scale = 1.0;
var accumulation = 0;
var maxAccumulation = 10;
var fps = 20;

init();

function init() {
    camera = new THREE.PerspectiveCamera(60, window.innerWidth / window.innerHeight, 1, 1000);
    camera.position.z = 2;

    cameraMatrix = {
        matrix: [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, -1, 1]
    };
    controls = new THREE.OrbitControls(camera);
    controls.rotateSpeed = 1.0;
    controls.zoomSpeed = 1.2;
    controls.panSpeed = 0.8;
    controls.noZoom = false;
    controls.noPan = false;
    controls.staticMoving = true;
    controls.dynamicDampingFactor = 0.3;
    window.addEventListener('resize', onWindowResize, false);
}

function onWindowResize() {
    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
}

function doRequest(method, url, callback, body) {
    var oReq = new XMLHttpRequest();
    var bodyStr;
    oReq.onload = callback;
    oReq.withCredentials = false;
    oReq.open(method, url, true);
    if (body) {
        oReq.setRequestHeader('Content-Type', 'application/json');
        bodyStr = JSON.stringify(body);
    }
    oReq.send(bodyStr);
    return oReq.response;
}

var statusImage = setInterval(function getImage() {
    if( !firstCameraRetrieved ) {
        doRequest('GET', braynsUrl + 'zerobuf/render/camera', function (event) {
            var res = JSON.parse(event.target.responseText);
            initialCameraPos.copy(res.origin);
            initialCameraTarget.copy(res.lookAt);

            var dir = new THREE.Vector3(
                res.lookAt.x-res.origin.x,
                res.lookAt.y-res.origin.y,
                res.lookAt.z-res.origin.z);
            scale = dir.length()/2.0;

            console.log('Origin: ' + JSON.stringify(initialCameraPos));
            console.log('Target: ' + JSON.stringify(initialCameraTarget));
            console.log('Scale : ' + scale);
            firstCameraRetrieved = true;
        });
    }
    
    controls.update();
    if( firstCameraRetrieved ) {
        if ( currentCameraPos.x !== camera.position.x) {
            var jsonCamera = { 'origin' : {}, 'lookAt': {}, 'up': {} };
            jsonCamera['origin']['x'] = initialCameraTarget.x + scale * -camera.position.x;
            jsonCamera['origin']['y'] = initialCameraTarget.y + scale * camera.position.y;
            jsonCamera['origin']['z'] = initialCameraTarget.z + scale * -camera.position.z;
            jsonCamera['lookAt']['x'] = initialCameraTarget.x;
            jsonCamera['lookAt']['y'] = initialCameraTarget.y;
            jsonCamera['lookAt']['z'] = initialCameraTarget.z;
            jsonCamera['up']['x'] = 0;
            jsonCamera['up']['y'] = 1;
            jsonCamera['up']['z'] = 0;
            doRequest('PUT', braynsUrl + 'zerobuf/render/camera', function () {}, jsonCamera);
            currentCameraPos.copy( camera.position );
            accumulation = 0;
        }

        if( accumulation < maxAccumulation ) 
        {
            doRequest('GET', braynsUrl + 'zerobuf/render/imagejpeg', function (event) {
                if (event.target.status === 200) {
                    var jsonObject = JSON.parse(event.target.responseText);
                    renderedImage.src = "data:image/jpg;base64," + jsonObject.data;
                    accumulation = accumulation + 1;
                }
            });
        }
    }
}, 1000/fps);

function refreshMainFrame() {
    console.log("refreshMainFrame");
    var frame = document.getElementById('mainFrame');
    document.getElementById('mainFrame').style.display = "inherit"
    frame.contentWindow.location.reload(true)
};
