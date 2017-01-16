from __future__ import print_function

import ctypes
import sys
from sys import platform as _platform
import os
import numpy as np
lib_path = os.path.abspath(os.path.join('..', '..'))
sys.path.append(lib_path)
import egs
from egs import DisplayListElem, PluginWrapper, Context, SerializedDisplayListElem
lib_path = os.path.join(os.path.join(os.environ.get('EGS_PATH'), 'plugins/sphere_plugin/'))
sys.path.append(lib_path)
from sphere_plugin import Sphere
lib_path = os.path.join(os.path.join(os.environ.get('EGS_PATH'), 'plugins/cylinder_plugin/'))
sys.path.append(lib_path)
from cylinder_plugin import Cylinder


LIB_SUFFIX = 'so'
if _platform == "darwin":
    LIB_SUFFIX = 'dylib'

LIB_NAME = 'libcalculate_bonds.'+LIB_SUFFIX


class Molecule:
    atoms = {"H": (0.230000, 0xffffff), "HE": (0.930000, 0xd9ffff), "LI": (0.680000, 0xcc80ff), "BE": (0.350000, 0xc2ff00), "B": (0.830000, 0xffb5b5), "C": (0.680000, 0x909090), "N": (0.680000, 0x3050f8), "O": (0.680000, 0xff0d0d), "F": (0.640000, 0x90e050), "NE": (1.120000, 0xb3e3f5), "NA": (0.970000, 0xab5cf2), "MG": (1.100000, 0x8aff00), "AL": (1.350000, 0xbfa6a6), "SI": (1.200000, 0xf0c8a0), "P": (0.750000, 0xff8000), "S": (1.020000, 0xffff30), "CL": (0.990000, 0x1ff01f), "AR": (1.570000, 0x80d1e3), "K": (1.330000, 0x8f40d4), "CA": (0.990000, 0x3de100), "SC": (1.440000, 0xe6e6e6), "TI": (1.470000, 0xbfc2c7), "V": (1.330000, 0xa6a6ab), "CR": (1.350000, 0x8a99c7), "MN": (1.350000, 0x9c7ac7), "FE": (1.340000, 0xe06633), "CO": (1.330000, 0xf090a0), "NI": (1.500000, 0x50d050), "CU": (1.520000, 0xc88033), "ZN": (1.450000, 0x7d80b0), "GA": (1.220000, 0xc28f8f), "GE": (1.170000, 0x668f8f), "AS": (1.210000, 0xbd80e3), "SE": (1.220000, 0xe1a100), "BR": (1.210000, 0xa62929), "KR": (1.910000, 0x5cb8d1), "RB": (1.470000, 0x702eb0), "SR": (1.120000, 0xff00), "Y": (1.780000, 0x94ffff), "ZR": (1.560000, 0x94e0e0), "NB": (1.480000, 0x73c2c9), "MO": (1.470000, 0x54b5b5), "TC": (1.350000, 0x3b9e9e), "RU": (1.400000, 0x248f8f), "RH": (1.450000, 0xa7d8c), "PD": (1.500000, 0x6985), "AG": (1.590000, 0xc0c0c0), "CD": (1.690000, 0xffd98f), "IN": (1.630000, 0xa67573), "SN": (1.460000, 0x668080), "SB": (1.460000, 0x9e63b5), "TE": (1.470000, 0xd47a00), "I": (1.400000, 0x940094), "XE": (1.980000, 0x429eb0), "CS": (1.670000, 0x57178f), "BA": (1.340000, 0xc900), "LA": (1.870000, 0x70d4ff), "CE": (1.830000, 0xffffc7), "PR": (1.820000, 0xd9e1c7), "ND": (1.810000, 0xc7e1c7), "PM": (1.800000, 0xa3e1c7), "SM": (1.800000, 0x8fe1c7), "EU": (1.990000, 0x61e1c7), "GD": (1.790000, 0x45e1c7), "TB": (1.760000, 0x30e1c7), "DY": (1.750000, 0x1fe1c7), "HO": (1.740000, 0xe19c), "ER": (1.730000, 0xe675), "TM": (1.720000, 0xd452), "YB": (1.940000, 0xbf38), "LU": (1.720000, 0xab24), "HF": (1.570000, 0x4dc2ff), "TA": (1.430000, 0x4da6ff), "W": (1.370000, 0x2194d6), "RE": (1.350000, 0x267dab), "OS": (1.370000, 0x266696), "IR": (1.320000, 0x175487), "PT": (1.500000, 0xd0d0e0), "AU": (1.500000, 0xffd123), "HG": (1.700000, 0xb8b8d0), "TL": (1.550000, 0xa6544d), "PB": (1.540000, 0x575961), "BI": (1.540000, 0x9e4fb5), "PO": (1.680000, 0xab5c00), "AT": (1.700000, 0x754f45), "RN": (2.400000, 0x428296), "FR": (2.000000, 0x420066), "RA": (1.900000, 0x7d00), "AC": (1.880000, 0x70abfa), "TH": (1.790000, 0xbaff), "PA": (1.610000, 0xa1ff), "U": (1.580000, 0x8fff), "NP": (1.550000, 0x80ff), "PU": (1.530000, 0x6bff), "AM": (1.510000, 0x545cf2), "CM": (1.500000, 0x785ce3), "BK": (1.500000, 0x8a4fe3), "CF": (1.500000, 0xa136d4), "ES": (1.500000, 0xb31fd4), "FM": (1.500000, 0xb31fba), "MD": (1.500000, 0xb30da6), "NO": (1.500000, 0xbd0d87), "LR": (1.500000, 0xc70066), "RF": (1.600000, 0xcc0059), "DB": (1.600000, 0xd1004f), "SG": (1.600000, 0xd90045), "BH": (1.600000, 0xe00038), "HS": (1.600000, 0xe6002e), "MT": (1.600000, 0xeb0026), "DS": (1.600000, 0xff00ff), "RG": (1.600000, 0xff00ff), "CN": (1.600000, 0xff00ff), "UUT": (1.600000, 0xff00ff), "UUQ": (1.600000, 0xff00ff), "UUP": (1.600000, 0xff00ff), "UUH": (1.600000, 0xff00ff), "UUS": (1.600000, 0xff00ff), "UUO": (1.600000, 0xff00ff)}
    apply_fun = None
    plugin_name = ctypes.c_char_p("molecule_plugin".encode('utf-8'))
    plugin_instances = {}
    plugin_instance_ctr = 0

    def __init__(self, filename, bonds_param=1.5):
        with open(filename, "r") as f:
            data = f.read().splitlines()
        num_atoms = int(data[0].strip())
        positions = np.zeros((num_atoms, 3), np.float)
        colors = np.zeros(num_atoms, np.long)
        for i, line in enumerate(data[2:]):
            atom, x, y, z = line.strip().split()
            atom = atom.upper()
            if atom not in Molecule.atoms:
                print("element not found: {} (at ({}, {}, {}))".format(atom, x, y, z))
            positions[i] = float(x), float(y), float(z)
            colors[i] = Molecule.atoms[atom][1]
        start = ctypes.POINTER(ctypes.c_float)()
        end = ctypes.POINTER(ctypes.c_float)()
        bond_lib = ctypes.CDLL(os.path.join(os.environ.get('EGS_PATH'), 'plugins', 'molecule_plugin', LIB_NAME))
        bond_lib.calc_bonds.argtypes = [ctypes.POINTER(ctypes.c_float),
                                        ctypes.POINTER(ctypes.c_float),
                                        ctypes.POINTER(ctypes.c_float),
                                        ctypes.c_int, ctypes.c_float,
                                        ctypes.POINTER(ctypes.POINTER(ctypes.c_float)),
                                        ctypes.POINTER(ctypes.POINTER(ctypes.c_float))]
        bond_lib.calc_bonds.restype = ctypes.c_int
        num_bonds = bond_lib.calc_bonds((ctypes.c_float * num_atoms)(*positions[:,0]),
                                        (ctypes.c_float * num_atoms)(*positions[:,1]),
                                        (ctypes.c_float * num_atoms)(*positions[:,2]),
                                        num_atoms, bonds_param, ctypes.byref(start), ctypes.byref(end))
        bonds_start = np.ctypeslib.as_array((ctypes.c_float * 3 * num_bonds).from_address(ctypes.addressof(start.contents)))
        bonds_end = np.ctypeslib.as_array((ctypes.c_float * 3 * num_bonds).from_address(ctypes.addressof(end.contents)))
        bonds = [bonds_start.reshape(num_bonds, 3), bonds_end.reshape(num_bonds, 3)]

        self._data = PluginWrapper()
        self._data.plugin_name = Molecule.plugin_name
        self.spheres = Sphere(positions, [0.31]*(len(positions)), colors).display_list_elem_ref
        self.cylinder = Cylinder(bonds[0], bonds[1], [0.046629]*len(bonds[0]), 0xffffffff).display_list_elem_ref
        self._data.data_length = ctypes.sizeof(ctypes.c_uint32)
        self._data.data = (ctypes.c_uint8 * self._data.data_length)(Molecule.plugin_instance_ctr)
        Molecule.plugin_instances[Molecule.plugin_instance_ctr] = self
        Molecule.plugin_instance_ctr += 1
        self._molecule_ref = self._data.get_display_list_elem()
        egs.printf("created molecule".encode('utf-8'), egs.DEBUG)

    @property
    def display_list_elem_ref(self):
        return self._molecule_ref

    @staticmethod
    def apply(ctx_ref, data_size, data, plugin_data):
        instance = ctypes.cast(data, ctypes.POINTER(ctypes.c_uint32))[0]
        if instance not in Molecule.plugin_instances:
            egs.printf("molecule plugin instance not found", egs.WARNING)
        else:
            Context.call_apply(ctx_ref, Molecule.plugin_instances[instance].spheres)
            Context.call_apply(ctx_ref, Molecule.plugin_instances[instance].cylinder)

    @staticmethod
    def terminate(data_size, data, plugin_data):
        instance = ctypes.cast(data, ctypes.POINTER(ctypes.c_uint32))[0]
        if instance not in Molecule.plugin_instances:
            egs.printf("molecule plugin instance not found", egs.WARNING)
        else:
            egs.printf("molecule destructor: calling sphere destructor", egs.DEBUG)
            Context.call_terminate(Molecule.plugin_instances[instance].spheres)
            egs.printf("molecule destructor: calling cylinder destructor", egs.DEBUG)
            Context.call_terminate(Molecule.plugin_instances[instance].cylinder)
            del instance
            egs.printf("molecule destructor: done", egs.DEBUG)

    @staticmethod
    def from_xyz_file(useless_context, filename):
        tmp = Molecule(filename)
        return ctypes.addressof(tmp.display_list_elem_ref.contents)


molecule_create_fun = ctypes.CFUNCTYPE(ctypes.c_void_p, ctypes.POINTER(Context), ctypes.c_char_p)
wrapped_molecule_create_fun = molecule_create_fun(Molecule.from_xyz_file)


def molecule_plugin_init_plugin(ctx_ref):
    Molecule.apply_fun = PluginWrapper.apply_fun_callback(Molecule.apply)
    Molecule.terminate_fun = PluginWrapper.terminate_fun_callback(Molecule.terminate)
    PluginWrapper.register_c_plugin(Molecule.plugin_name, Molecule.apply_fun, Molecule.terminate_fun)

    Context.load_plugin_ptr(ctx_ref, "sphere_plugin".encode('utf-8'))
    Context.load_plugin_ptr(ctx_ref, "cylinder_plugin".encode('utf-8'))
    Context.register_py_plugin_function(ctx_ref, Molecule.plugin_name, Molecule.from_xyz_file.__name__.encode('utf-8'), wrapped_molecule_create_fun)
    return True


def molecule_plugin_terminate_plugin():
    egs.printf("Molecule plugin terminated")

