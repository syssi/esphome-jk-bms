from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import asyncio
from unittest import mock

from esphome.core import CORE

from components.jk_bms_ble import select as selects


def test_lookup_arrays_are_emitted_again_after_core_reset():
    CORE.reset()
    emitted = []
    config = {selects.CONF_JK_BMS_BLE_ID: "hub", selects.CONF_UART1_PROTOCOL: {}}
    with (
        mock.patch.object(
            selects.cg, "get_variable", new=mock.AsyncMock(return_value=mock.Mock())
        ),
        mock.patch.object(
            selects.select, "new_select", new=mock.AsyncMock(return_value=mock.Mock())
        ),
        mock.patch.object(selects.cg, "register_component", new=mock.AsyncMock()),
        mock.patch.object(selects.cg, "add"),
        mock.patch.object(
            selects.cg,
            "add_global",
            side_effect=lambda expression: emitted.append(str(expression)),
        ),
    ):
        asyncio.run(selects.to_code(config))
        asyncio.run(selects.to_code(config))
        assert len(emitted) == 1
        CORE.reset()
        asyncio.run(selects.to_code(config))
        assert len(emitted) == 2
    CORE.reset()
