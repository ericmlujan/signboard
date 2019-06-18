#!/usr/bin/env bash

# set -euf -o pipefail

# deploys and builds this project on a remote host
# NOTE: must be run from the project root for now
DEPLOY_TARGET_HOSTNAME='raspberrypi'
DEPLOY_TARGET_USER=$USER
DEPLOY_TARGET_DIR='~/signboard'

BRANCH=`git rev-parse --abrev-ref HEAD`
if [ $? -eq 1 ] || [ $BRANCH = 'HEAD' ]
then
    echo "Current HEAD is not associated with a named git ref"
    exit 1
fi

# copy local sources to the remote
rsync -av . $DEPLOY_TARGET_USER@$DEPLOY_TARGET_HOSTNAME:$DEPLOY_TARGET_DIR

# over ssh target a clean build
ssh $DEPLOY_TARGET_USER@$DEPLOY_TARGET_HOSTNAME "cd ${DEPLOY_TARGET_DIR}; bazel build $1"
