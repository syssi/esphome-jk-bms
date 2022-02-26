#!/bin/bash

esphome -s external_components_source components ${1:-run} esp32-example-faker.yaml
