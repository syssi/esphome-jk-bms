#!/bin/bash

REPO_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)

esphome -s external_components_source "$REPO_DIR/components" "${1:-run}" "${2:-$REPO_DIR/esp8266-example-faker.yaml}"
