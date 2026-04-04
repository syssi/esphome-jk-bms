"""Mock missing esphome external component dependencies for test collection."""

import sys

import esphome.codegen as cg


def _make_modbus_mock(ns_name, *class_names):
    """Return a namespace where Device classes are MockObjClass and functions return {}."""
    _ns = cg.esphome_ns.namespace(f"_mock_{ns_name}")
    _classes = {name: _ns.class_(name) for name in class_names}

    class _MockModbus:
        def __getattr__(self, name):
            if name in _classes:
                return _classes[name]
            return lambda *a, **kw: {}

    return _MockModbus()


sys.modules["esphome.components.jk_balancer_modbus"] = _make_modbus_mock(
    "jk_balancer_modbus", "JkBalancerModbusDevice"
)
sys.modules["esphome.components.jk_modbus"] = _make_modbus_mock(
    "jk_modbus", "JkModbusDevice"
)
