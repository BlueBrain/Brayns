#! /bin/bash
mpiexec -host cave1,cave2,cave3,cave4,cave5,cave6,cave7,cave8 bash -c 'hostname -s; killall -q mpv'
