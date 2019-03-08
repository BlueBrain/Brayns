#! /bin/bash
DIRECTORY=`dirname $0`
mpiexec -host cave1,cave1,cave2,cave2,cave3,cave3,cave4,cave4,cave5,cave5,cave6,cave6,cave7,cave7,cave8,cave8 \
  -np 1 $DIRECTORY/launchmpv.sh 0 master : -np 1 $DIRECTORY/launchmpv.sh 1 : \
  -np 1 $DIRECTORY/launchmpv.sh 0        : -np 1 $DIRECTORY/launchmpv.sh 1 : \
  -np 1 $DIRECTORY/launchmpv.sh 0        : -np 1 $DIRECTORY/launchmpv.sh 1 : \
  -np 1 $DIRECTORY/launchmpv.sh 0        : -np 1 $DIRECTORY/launchmpv.sh 1 : \
  -np 1 $DIRECTORY/launchmpv.sh 0        : -np 1 $DIRECTORY/launchmpv.sh 1 : \
  -np 1 $DIRECTORY/launchmpv.sh 0        : -np 1 $DIRECTORY/launchmpv.sh 1 : \
  -np 1 $DIRECTORY/launchmpv.sh 0        : -np 1 $DIRECTORY/launchmpv.sh 1 : \
  -np 1 $DIRECTORY/launchmpv.sh 0        : -np 1 $DIRECTORY/launchmpv.sh 1
