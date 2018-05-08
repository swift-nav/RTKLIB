#!/bin/bash

# Copyright (C) 2018 Swift Navigation Inc.
# Contact: Swift Navigation <dev@swiftnav.com>
#
# This source is subject to the license found in the file 'LICENSE' which must
# be be distributed together with this source. All other rights reserved.
#
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
#
# Script for publishing built binaries to S3.

set -e

REPO="${PWD##*/}"
BUCKET="${BUCKET:-rtklib-ci-builds}"
PRS_BUCKET="${PRS_BUCKET:-rtklib-ci-builds}"

BUILD_VERSION="$(git describe --tags --dirty --always)"

if [ -z "$S3_PATH" ]; then
  echo "Exiting... no \$S3_PATH specified..."
  echo 0
fi

S3_PATH="$BUILD_VERSION/$S3_PATH"

echo "Comment PULL_REQUEST ($TRAVIS_PULL_REQUEST)"
echo "Comment BRANCH ($TRAVIS_BRANCH)"
echo "Comment TAG ($TRAVIS_TAG)"

if [ "$TRAVIS_PULL_REQUEST" == "false" ]; then
    if [[ "$TRAVIS_BRANCH" == master || "$TRAVIS_TAG" == v* || "$TRAVIS_BRANCH" == v*-release ]]; then
        COMMENT="$S3_PATH
https://console.aws.amazon.com/s3/home?region=us-west-2&bucket=$BUCKET&prefix=$S3_PATH/"
        URL="https://slack.com/api/chat.postMessage?token=$SLACK_TOKEN&channel=$SLACK_CHANNEL"
        DATA="text=$COMMENT"
        if [ ! -z "$SLACK_CHANNEL" ]; then
          curl --data-urlencode "$DATA" "$URL"
        else
          echo "Not posting to Slack, no \$SLACK_CHANNEL specified..."
        fi
    fi
elif [ ! -z "$GITHUB_TOKEN" ]; then
    COMMENT=\
"## $BUILD_VERSION\n"\
"+ [Artifacts (S3 / AWS Console)](https://console.aws.amazon.com/s3/home?region=us-west-2&bucket=rtklib-ci-builds&prefix=$S3_PATH/)\n"\
"+ s3://$PRS_BUCKET/$S3_PATH"
    URL="https://api.github.com/repos/swift-nav/$REPO/issues/$TRAVIS_PULL_REQUEST/comments"
    curl -u "$GITHUB_TOKEN:" -X POST "$URL" -d "{\"body\":\"$COMMENT\"}"
fi
