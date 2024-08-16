#!/bin/bash

if [[ $2 == tests/* ]]; then
  C="../components"
else
  C="components"
fi

esphome -s mac_address 20:22:09:27:38:60 -s external_components_source $C ${1:-run} ${2:-esp32-example-faker.yaml}
