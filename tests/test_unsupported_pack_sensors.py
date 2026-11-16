"""Active balancers do not measure pack current or power."""

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from esphome.core import CORE
import pytest
import voluptuous as vol

from components.heltec_balancer_ble import sensor as heltec
from components.jk_balancer import sensor as jk


@pytest.fixture(autouse=True)
def reset_core():
    CORE.reset()
    yield
    CORE.reset()


@pytest.mark.parametrize(
    "module, key", [(jk, "current"), (jk, "power"), (heltec, "current")]
)
def test_unsupported_pack_measurement_is_rejected(module, key):
    with pytest.raises(vol.Invalid):
        module.CONFIG_SCHEMA({key: {"name": "Unsupported measurement"}})


@pytest.mark.parametrize("module", [jk, heltec])
def test_balancing_current_remains_supported(module):
    assert "balancing_current" in module.CONFIG_SCHEMA(
        {"balancing_current": {"name": "Balance current"}}
    )
