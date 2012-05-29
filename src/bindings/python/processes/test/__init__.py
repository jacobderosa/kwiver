#ckwg +5
# Copyright 2012 by Kitware, Inc. All Rights Reserved. Please refer to
# KITWARE_LICENSE.TXT for licensing information, or contact General Counsel,
# Kitware, Inc., 28 Corporate Drive, Clifton Park, NY 12065.


from vistk.processes.test import examples


def __vistk_register__():
    from vistk.pipeline import process_registry

    module_name = 'python:test.examples'

    reg = process_registry.ProcessRegistry.self()

    if reg.is_module_loaded(module_name):
        return

    reg.register_process('test_python_process', 'A test Python process.', examples.TestPythonProcess)

    reg.mark_module_as_loaded(module_name)
