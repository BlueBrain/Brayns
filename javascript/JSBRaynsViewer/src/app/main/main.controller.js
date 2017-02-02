/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nicolas Antille <nicolas.antille@epfl.ch>
 *                     Olivier Amblet <olivier.amblet@epfl.ch>
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
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

(function () {
    'use strict';

    angular
        .module('visualisationUi')
        .controller('MainController', MainController);

    // Variables
    var serverHostname = 'bbpav03.epfl.ch';
    var braynsSetAttrib = 'zerobuf/render/attribute';
    var braynsSetCamera = 'zerobuf/render/fovcamera';
    var braynsGetCamera = braynsSetCamera;
    var braynsResetCamera = 'zerobuf/render/reset';
    var braynsGetImage = 'zerobuf/render/imagejpeg';


    /** @ngInject */
    function MainController($scope, $http, $log) {
        var vm = this;
        
        vm.fps = 5;
        vm.visualisationScale = 1.0;
        
        vm.streamURL = 'http://' + serverHostname + ':8385/image-streaming-service/v1/image_streaming_feed/demo';
        vm.serverURL = 'http://' + serverHostname + ':5000/';
        vm.visualisation = vm.streamURL;
        
        vm.circuitBuilding = {value: 8, min: 0, max: 8, step:1, label:'Play'};
        vm.simulation = {min: 80000, max: 88000, step:1, label:'Play simulation'};
        vm.active = {panel: 'about', preset:'exobj', materialMode:'diffuse', shadowMode:'no-shadow', timeline:vm.circuitBuilding};
        
        // Camera attributes
        vm.animation = false;
        vm.timestamp = 0.0;
        vm.camera_aperture = 0.0;
        vm.active.camera_aperture = {value: 0, min: 0, max: 50, step:1};
        vm.camera_focal_length = 1.0;
        vm.active.camera_focal_length = {value: 1, min: 1, max: 500, step:5};
        
        // Background Color
        vm.bgColor = 0;
        vm.active.bgcolor = {min: 0, max: 100, step:1};
        
        // Epsilon
        vm.epsilon = 1.0;
        vm.active.epsilon = {min: 0, max: 10, step:1}

        // Jpeg quality
        vm.jpegquality = 50;
        vm.active.jpegquality = {value: 30, min: 10, max: 100, step:10}

        // Samples per pixel
        vm.spp = 1.0;
        vm.active.spp = {min: 0, max: 128, step:4}

        // Camera
        vm.cameraMatrix = new THREE.Matrix4();
        vm.positionChangeCounter = 0;
        vm.currentCameraPos = new THREE.Vector3(0, 0, 0);
        vm.initialCameraPos = new THREE.Vector3(0, 0, 0);
        vm.initialCameraTarget = new THREE.Vector3(0, 0, 0);
        vm.camera = new THREE.PerspectiveCamera(60, window.innerWidth / window.innerHeight, 1, 1000);
        vm.camera.position.z = 1;

        vm.cameraMatrix = {
            matrix: [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, -1, 1]
        };
        
        // Controls
        vm.controls = new THREE.OrbitControls(vm.camera);
        vm.controls.rotateSpeed = 0.5;
        vm.controls.zoomSpeed = 0.03;
        vm.controls.panSpeed = 0.1;
        vm.controls.noZoom = false;
        vm.controls.noPan = false;
        vm.controls.staticMoving = true;
        vm.controls.dynamicDampingFactor = 0.3;
        window.addEventListener('resize', resizeWindow, false);
        
        vm.failed_connection_attempts = 0
        
        // This method sends parameters to the server via PUT
        vm.sendParameter = function(key, value) {
            var jsonData = {
                'key': key,
                'value': value
            };
            doRequest('PUT', vm.serverURL + braynsSetAttrib, function() {}, jsonData);
        }
        
        function boolToInt (value) {
            return value ? 1 : 0;
        }

        function resizeWindow() {
            vm.camera.aspect = window.innerWidth / window.innerHeight;
            vm.camera.updateProjectionMatrix();
            var width = window.innerWidth;
            var height = window.innerHeight;
            var jsonData = {
                'key': 'jpeg-size',
                'value': width + ' ' + height
            };
            doRequest('PUT', vm.serverURL + braynsSetAttrib, function () {}, jsonData);
            jsonData = {
                'key': 'window-size',
                'value': width + ' ' + height
            };
            doRequest('PUT', vm.serverURL + braynsSetAttrib, function () {}, jsonData);
        }
        
        // Listeners
        function activePanelChange(newValue, oldValue) {
            console.log('panel:', newValue);
        }
        
        function activePresetChange (newValue, oldValue) {
            console.log('preset:', newValue);
            switch (newValue) {
                case 'exobj':
                    vm.active.timeline = vm.circuitBuilding;
                    break;
                case 'proximityrenderer':
                    break;
                case 'simulationrenderer':
                    vm.active.timeline = vm.simulation;
                    break;
            }
            vm.sendParameter('renderer', newValue);
        }
        
        function activeMaterialModeChange(newValue, oldValue) {
            console.log('material mode:', newValue);
            vm.sendParameter('material', newValue);
        }
        
        function activeShadowModeChange(newValue, oldValue) {
            console.log('shadow mode:', newValue);
            var shadows = newValue !== 'no-shadow';
            var softShadow = newValue == 'soft-shadow';
            vm.sendParameter('shadows', boolToInt(shadows));
            vm.sendParameter('soft-shadows', boolToInt(softShadow));
        }
        
        function timeStampChange(newValue, oldValue) {
            if (newValue !== undefined) {
                console.log('timestamp :', newValue.toString());
                if( newValue === 8 ) {
                    vm.sendParameter('timestamp', 300);
                } else {
                    vm.sendParameter('timestamp', newValue.toString());
                }
            }
        }

        function ambientOcclusionChange(newValue, oldValue) {
            if (newValue !== undefined) {
                console.log('ambient occlusion :', (newValue/10.0).toString());
                vm.sendParameter('ambient-occlusion', (newValue/10.0).toString());
            }
        }
        
        function cameraApertureChange(newValue, oldValue) {
            if (newValue !== undefined) {
                vm.camera_aperture = newValue / 1000.0;
                console.log('Aperture: ' + vm.camera_aperture );
                setServerCamera();
            }
        }

        function cameraFocalLengthChange(newValue, oldValue) {
            if (newValue !== undefined) {
                vm.camera_focal_length = newValue;
                console.log('Focal length: ' + vm.camera_focal_length );
                setServerCamera();
            }
        }
        
        function sendBackgroundColor() {
            vm.sendParameter('background-color', (vm.bgColor/100.0).toString() + ' ' + (vm.bgColor/100.0).toString() + ' ' + (vm.bgColor/100.0).toString() );
        }

        function bgColorChange(newValue, oldValue) {
            if (newValue !== undefined) {
                vm.bgColor = newValue;
                sendBackgroundColor();
            }
        }

        function epsilonChange(newValue, oldValue) {
            if (newValue !== undefined) {
                vm.epsilon = newValue;
                vm.sendParameter('epsilon', (Math.pow(10,-newValue)).toString());
            }
        }

        function jpegQualityChange(newValue, oldValue) {
            if (newValue !== undefined) {
                vm.jpegquality = newValue;
                vm.sendParameter('jpeg-compression', newValue.toString());
            }
        }

        function sppChange(newValue, oldValue) {
            if (newValue !== undefined) {
                vm.spp = newValue;
                vm.sendParameter('spp', (vm.spp).toString());
            }
        }

        function cameraChange(newValue, oldValue) {
            if (newValue !== undefined) {
                console.log(newValue);
            }
        }

        vm.timelineInterval = undefined;
        vm.playToggle = function(newValue, oldValue) {
            if (newValue == 1) {
                vm.timelineInterval = setInterval(vm.nextTimeStep, 1000 / vm.fps);
                vm.active.timeline.label = 'Pause simulation';
            }
            else {
                vm.stopPlayback();
                vm.active.timeline.label = 'Play simulation';
            }
        }
        
        vm.stopPlayback = function() {
            clearInterval(vm.timelineInterval);
            vm.timelineInterval = undefined;
        }
        
        vm.nextTimeStep = function(){
            $scope.$digest();
            if (vm.active.timeline.value === vm.active.timeline.max || vm.active.timeline.value === undefined) {
                vm.active.timeline.value = vm.active.timeline.min;
            }
            else {
                vm.active.timeline.value += 1;
            }
        }
        
        // Registering listeners
        $scope.$watch('vm.active.panel', activePanelChange);
        $scope.$watch('vm.active.preset', activePresetChange);
        $scope.$watch('vm.active.materialMode', activeMaterialModeChange);
        $scope.$watch('vm.active.shadowMode', activeShadowModeChange);
        $scope.$watch('vm.active.timeline.value', timeStampChange);
        $scope.$watch('vm.active.ambientocclusion.value', ambientOcclusionChange);
        $scope.$watch('vm.active.camera_aperture.value', cameraApertureChange);
        $scope.$watch('vm.active.camera_focal_length.value', cameraFocalLengthChange);
        $scope.$watch('vm.active.bgcolor.value', bgColorChange);
        $scope.$watch('vm.active.epsilon.value', epsilonChange);
        $scope.$watch('vm.active.jpegquality.value', jpegQualityChange);
        $scope.$watch('vm.active.spp.value', sppChange);
        $scope.$watch('vm.sliders.playToggle', vm.playToggle);
        $scope.$watch('vm.active.camera_reset', cameraChange);
        
        $scope.resetCamera = function() {
            reinitaliseServerCamera();
        };
        
        $scope.globalCamera = function() {
            vm.camera.position.x = 1;
            vm.camera.position.y = 0;
            vm.camera.position.z = 0;
        };
        
        $scope.closeUpCamera = function() {
            vm.camera.position.x = 0.1;
            vm.camera.position.y = 0;
            vm.camera.position.z = 0;
        };
        
        $scope.autoCamera = function() {
            vm.animation = !vm.animation;
        };
        
        function doRequest(method, url, callback, body) {
            var oReq = new XMLHttpRequest();
            var bodyStr;
            oReq.onload = callback;
            oReq.withCredentials = false;
            oReq.open(method, url, true);
            if (body) {
                bodyStr = JSON.stringify(body);
            }
            oReq.onerror = function(e) {
                vm.failed_connection_attempts += 1;
            }
            var result = oReq.send(bodyStr);
            if (method !== 'PUT' && angular.isUndefined(result)) {
                console.log('Could not connect to send parameters to the server.')
            }
            return oReq.response;
        }
        
        function reinitaliseServerCamera() {
            console.log('Reset camera');
            var jsonData = {'camera': true};
            doRequest('PUT', vm.serverURL + braynsResetCamera, getServerCamera, jsonData);
        }
                      
        function getServerCamera() {
            doRequest('GET', vm.serverURL + braynsGetCamera, updateJsCamera);
        }
                    
        function updateJsCamera (event) {
            var res = JSON.parse(event.target.responseText);
            vm.initialCameraPos.copy(res.origin);
            vm.initialCameraTarget.copy(res.lookAt);

            var dir = new THREE.Vector3(
                res.lookAt.x - res.origin.x,
                res.lookAt.y - res.origin.y,
                res.lookAt.z - res.origin.z);
            vm.imageScale = dir.length() / 2.0;

            console.log('Origin: ' + JSON.stringify(vm.initialCameraPos));
            console.log('Target: ' + JSON.stringify(vm.initialCameraTarget));
            console.log('Scale : ' + vm.imageScale);
            
            vm.camera.position.x = 0;
            vm.camera.position.y = 0;
            vm.camera.position.z = 1;
        }
        
        function setServerCamera() {
            var jsonCamera = {
                'origin': {},
                'lookAt': {},
                'up': {}
            };
            jsonCamera['origin']['x'] = vm.initialCameraTarget.x + vm.imageScale * -vm.camera.position.x;
            jsonCamera['origin']['y'] = vm.initialCameraTarget.y + vm.imageScale * vm.camera.position.y;
            jsonCamera['origin']['z'] = vm.initialCameraTarget.z + vm.imageScale * -vm.camera.position.z;
            jsonCamera['lookAt']['x'] = vm.initialCameraTarget.x;
            jsonCamera['lookAt']['y'] = vm.initialCameraTarget.y;
            jsonCamera['lookAt']['z'] = vm.initialCameraTarget.z;
            jsonCamera['up']['x'] = 0;
            jsonCamera['up']['y'] = 1;
            jsonCamera['up']['z'] = 0;
            jsonCamera['fovFocalLength'] = vm.camera_focal_length;
            jsonCamera['fovAperture'] = vm.camera_aperture;
            doRequest('PUT', vm.serverURL + braynsSetCamera, function () {}, jsonCamera);
            vm.currentCameraPos.copy(vm.camera.position);
        }
        
        var status = setInterval(function updateCamera() {
            // Send the camera to the server so that it matches with the app on the server
            vm.controls.update();
            
            if (vm.animation) {
                vm.camera.position.x = Math.cos(1.3*vm.timestamp);
                vm.camera.position.y = 0;
                vm.camera.position.z = Math.sin(vm.timestamp);
                vm.timestamp += 0.01;
            }
            
            if (vm.currentCameraPos.x !== vm.camera.position.x) {
                console.log(vm.camera.position);
                setServerCamera();
            }
        }, 100);
        
        // Main initialization
        reinitaliseServerCamera();
        resizeWindow();
    }
})();